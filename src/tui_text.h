#pragma once
#include "tui_surface.h"
#include "tui_appearance.h"
#include "tui_text_utils.h"
#include "tui_active_element.h"
#include "tui_input.h"
#include "tui_scroll.h"

#include <algorithm>

namespace tui
{
	struct text_appearance_a
	{
		scroll_appearance_a text_scroll_appearance_a;

		text_appearance_a() : text_scroll_appearance_a(tui::DIRECTION::VERTICAL) {}
		text_appearance_a(scroll_appearance_a scroll) : text_scroll_appearance_a(scroll) {}

		void setColor(color Color)
		{
			text_scroll_appearance_a.setColor(Color);
		}
	};

	struct text_appearance : appearance
	{
	protected:
		text_appearance_a active_appearance;
		text_appearance_a inactive_appearance;
	public:
		text_appearance() 
		{
			inactive_appearance.setColor(tui::COLOR::DARKGRAY);
		}
		text_appearance(text_appearance_a active, text_appearance_a inactive) : active_appearance(active), inactive_appearance(inactive) {}

		void setColor(color Color) override
		{
			active_appearance.setColor(Color);
			inactive_appearance.setColor(Color);
			setAppearanceAction();
		}

		void setAppearance(text_appearance appearance) { setElement(*this, appearance); }
		text_appearance getAppearance() { return *this; }

		void setActiveAppearance(text_appearance_a active) { setElement(active_appearance, active); }
		text_appearance_a getActiveAppearance() { return active_appearance; }

		void setInactiveAppearance(text_appearance_a inactive) { setElement(inactive_appearance, inactive); }
		text_appearance_a getInactiveAppearance() { return inactive_appearance; }
	};

	struct text : surface, text_appearance, active_element
	{
	private:
		surface m_text;
		scroll<DIRECTION::VERTICAL> m_scroll;

		symbol_string m_unprepared_text;
		std::vector<vec2i> m_symbolPos;

		bool m_use_control_characters = true;
		bool m_display_scroll = true;

		bool m_redraw_needed = true;

		void fill()
		{
			m_text.makeTransparent();

			if (m_symbolPos.size() > 0)
			{
				auto f_it = std::lower_bound(
					m_symbolPos.begin(),
					m_symbolPos.end(),
					vec2i(0, m_scroll.getTopPosition()),
					[&](const vec2i& a, const vec2i& b) 
					{
						return a.y * m_text.getSize().x + a.x < b.y * m_text.getSize().x + b.x;
					}
				);	

				for (int i = f_it - m_symbolPos.begin(); i < m_unprepared_text.size(); i++)
				{
					vec2i p = m_symbolPos[i];
					p.y -= m_scroll.getTopPosition();

					if (p.y >= m_text.getSize().y) { break; }

					if (!isControl(utf8ToUtf32(m_unprepared_text[i].getCluster())[0]))
					{
						m_text.setSymbolAt(m_unprepared_text[i], p);
					}
				}
			}

			makeTransparent();
			insertSurface(m_text);
			if (m_display_scroll && m_scroll.isNeeded()) { insertSurface(m_scroll, false); }
		}
		void prepareText()
		{
			m_symbolPos.resize(m_unprepared_text.size());

			int pos = 0;
			for (int i = 0; i < m_unprepared_text.size(); i++)
			{
				int pos_in_line = pos % m_text.getSize().x;
				m_symbolPos[i] = { pos_in_line, pos / m_text.getSize().x };

				if (isControl(utf8ToUtf32(m_unprepared_text[i].getCluster())[0]))
				{
					if (m_use_control_characters)
					{
						switch (m_unprepared_text[i][0])
						{
						case '\n':
							pos += m_text.getSize().x - pos_in_line;
							break;
						case '\t':
							pos += 4;
						}
					}
					continue;
				}

				pos++;
			}
		}

		void adjustSizes()
		{
			m_text.setSizeInfo({ {0,0}, {100,100} });
			updateSurfaceSize(m_text);
			prepareText();
			if (m_display_scroll)
			{
				if (getNumberOfLines() > m_text.getSize().y)
				{
					m_text.setSizeInfo({ {-1,0}, {100,100} });
					updateSurfaceSize(m_text);
					prepareText();
				}
			}
			m_scroll.setContentLength(getNumberOfLines());
		}

		void resizeAction() override
		{
			updateSurfaceSize(m_scroll);
			adjustSizes();
			m_redraw_needed = true;
		}
		void updateAction() override { update(); }
		void drawAction() override
		{
			if (m_redraw_needed) { fill(); }
			m_redraw_needed = false;
		}

		void activationAction() override 
		{
			m_scroll.activate(); 
			m_redraw_needed = true;
		}
		void disactivationAction() override 
		{
			m_scroll.disactivate(); 
			m_redraw_needed = true;
		}

		void setAppearanceAction() override
		{
			m_scroll.setAppearance({active_appearance.text_scroll_appearance_a, inactive_appearance.text_scroll_appearance_a});
			m_redraw_needed = true;
		}

	public:
		short& key_up = m_scroll.key_up;
		short& key_down = m_scroll.key_down;
		short& key_pgup = m_scroll.key_pgup;
		short& key_pgdn = m_scroll.key_pgdn;

		text() : text({ {1,1} }) {}
		text(surface_size size) : text(size, U"") {}
		text(surface_size size, symbol_string txt) : m_scroll({ 0, 100 })
		{
			setSizeInfo(size);
			m_text.setSizeInfo({ {-1,0}, {100,100} });

			m_scroll.setPositionInfo(position({ 0,0 }, { 0,0 }, { POSITION::END, POSITION::BEGIN }));
			setText(txt);

			setAppearanceAction();
		}

		vec2i getSymbolPos(int i)
		{
			return m_symbolPos[i];
		}

		void setText(symbol_string txt)
		{
			m_unprepared_text = txt;
			adjustSizes();
			m_redraw_needed = true;
		}
		symbol_string getText() { return m_unprepared_text; }

		int getNumberOfLines()
		{
			if (m_symbolPos.size() > 0)
			{
				return m_symbolPos.back().y + 1;
			}
			return 0;
		}

		void goToLine(int line)
		{
			m_scroll.setTopPosition(line);
			m_redraw_needed = true;
		}

		//return current line number
		int getLine() { return m_scroll.getTopPosition(); }

		void lineUp()
		{
			m_scroll.up();
			m_redraw_needed = true;
		}
		void lineDown()
		{
			m_scroll.down();
			m_redraw_needed = true;
		}
		void pageUp()
		{
			m_scroll.pageUp();
			m_redraw_needed = true;
		}
		void pageDown()
		{
			m_scroll.pageDown();
			m_redraw_needed = true;
		}

		void useControlCharacters(bool use)
		{
			m_use_control_characters = use;
			adjustSizes();
			m_redraw_needed = true;
		}
		bool isUsingControlCharacters() { return m_use_control_characters; }

		void useScroll(bool display)
		{
			m_display_scroll = display;
			adjustSizes();
			m_redraw_needed = true;
		}
		//is displaying scroll if needed
		bool isUsingScroll() { return m_display_scroll; }

		//is displaying scroll currently
		bool isDisplayingScroll() { return m_display_scroll && m_scroll.isNeeded(); }

		void resizeToText(unsigned int max_width = 0)
		{
			if (max_width == 0)
			{
				bool use_c_char = m_use_control_characters;
				m_use_control_characters = false;//temporarily disable control characters

				setSizeInfo({ {(int)m_unprepared_text.size(), 1} });
				prepareText();

				m_use_control_characters = use_c_char;

				m_scroll.setContentLength(1);			
			}
			else
			{
				setSizeInfo({ {(int)max_width, 1} });
				prepareText();

				int h = m_symbolPos.back().y;

				m_scroll.setContentLength(h);
				setSizeInfo({ {(int)max_width, h} });
			}
			m_redraw_needed = true;
		}

		void update()
		{
			int pos = m_scroll.getTopPosition();
			m_scroll.update();
			if (pos != m_scroll.getTopPosition()) { m_redraw_needed = true; }
		}

	};
}