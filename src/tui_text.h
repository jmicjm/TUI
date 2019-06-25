#pragma once
#include "tui_console.h"
#include "tui_appearance.h"
#include "tui_text_utils.h"
#include "tui_scroll.h"

namespace tui
{
	struct text_appearance : appearance
	{
	protected:
		scroll_appearance m_active_scroll;
		scroll_appearance m_inactive_scroll;
	public:
		text_appearance() : text_appearance({ { U'\x2551', COLOR::WHITE }, { U'\x2502', COLOR::WHITE } },
											{ { U'\x2551', COLOR::DARKGRAY }, { U'\x2502', COLOR::DARKGRAY } }) {}
		text_appearance(scroll_appearance active, scroll_appearance inactive) : m_active_scroll(active), m_inactive_scroll(inactive) {}

		void setColor(color Color) override
		{
			m_active_scroll.setColor(Color);
			m_inactive_scroll.setColor(Color);
			setAppearance_action();
		}

		void setAppearance(text_appearance appearance)
		{
			*this = appearance;
			setAppearance_action();
		}
		text_appearance getAppearance() { return *this; }

		void setActiveScroll(scroll_appearance active)
		{
			m_active_scroll = active;
			setAppearance_action();
		}
		scroll_appearance getActiveScroll() { return m_active_scroll; }
		void setInactiveScroll(scroll_appearance inactive)
		{
			m_inactive_scroll = inactive;
			setAppearance_action();
		}
		scroll_appearance getInactiveScroll() { return m_inactive_scroll; }
	};

	struct text : surface, text_appearance, active_element
	{
	private:
		surface m_text;
		scroll<DIRECTION::VERTICAL> m_scroll;

		console_string m_unprepared_text;
		console_string m_prepared_text;

		bool m_use_prepared_text = true;
		bool m_use_dense_punctuation = false;
		bool m_use_control_characters = true;
		bool m_display_scroll = true;

		void fill()
		{
			m_text.makeTransparent();

			for (int i = 0; i < m_text.getSize().x; i++)
			{
				for (int j = 0; j < m_text.getSize().y; j++)
				{
					if (m_text.getSize().x * m_scroll.getHandlePosition() + j * m_text.getSize().x + i < m_prepared_text.size())
					{
						m_text[i][j] = m_prepared_text[m_text.getSize().x * m_scroll.getHandlePosition() + j * m_text.getSize().x + i];
					}
				}
			}

			makeTransparent();
			insertSurface(m_text);
			if (m_display_scroll && m_scroll.isNeeded()) { insertSurface(m_scroll); }
		}
		void prepareText()
		{
			console_string prepared;

			auto isControl = [](char32_t ch)
			{
				return GetGraphemeType(ch) == GRAPHEME_TYPE::CONTROL
					|| GetGraphemeType(ch) == GRAPHEME_TYPE::CR
					|| GetGraphemeType(ch) == GRAPHEME_TYPE::LF;
			};
			auto usePrepared = [&]()
			{
				return m_use_prepared_text && getSize().x > 2;
			};

			int pos = 0;
			for (int i = 0; i < m_unprepared_text.size(); i++)
			{
				int pos_in_line = pos % m_text.getSize().x;

				if (isControl(m_unprepared_text[i].getFirstChar()))
				{
					if (m_use_control_characters)
					{
						if (m_unprepared_text[i].getFirstChar() == '\n')
						{
							for (int i = 0; i < (m_text.getSize().x - pos_in_line); i++)
							{
								prepared += " ";
							}
							pos += m_text.getSize().x - pos_in_line;
						}
					}
					continue;
				}

				if (usePrepared()
					&& pos_in_line == m_text.getSize().x - 1
					&& i + 1 < m_unprepared_text.size()
					&& m_unprepared_text[i].getSymbol() != (U" ")
					&& m_unprepared_text[i + 1].getSymbol() != (U" ")
					&& !isControl(m_unprepared_text[i + 1].getFirstChar())
					&& (m_use_dense_punctuation ? !IsPunctuation(m_unprepared_text[i]) : true)
					&& (m_use_dense_punctuation ? !IsPunctuation(m_unprepared_text[i + 1]) : true)
					)
				{
					prepared << m_unprepared_text[i - 1].getColor();
					if (m_unprepared_text[i - 1].getSymbol() != U" ")
					{
						prepared << "-";
					}
					else { prepared << " "; }
					pos++;
				}
				if (usePrepared() && pos_in_line == 0 && m_unprepared_text[i].getSymbol() == U" ") { continue; }//omit space at start of line

				prepared += m_unprepared_text[i];
				pos++;
			}

			m_prepared_text = prepared;
		}

	public:
		int keyUp = KEYBOARD::KEY::UP;
		int keyDown = KEYBOARD::KEY::DOWN;
		int keyPageUp = KEYBOARD::KEY::PGUP;
		int keyPageDown = KEYBOARD::KEY::PGDN;

		text(surface_size size, console_string txt) : m_scroll({ 0, 100 })
		{
			setSize(size);
			m_text.setSize({ {-1,0}, {100,100} });

			m_scroll.setPosition(position({ 0,0 }, { 0,0 }, { POSITION::HORIZONTAL::RIGHT, POSITION::VERTICAL::TOP }));
			setText(txt);

			setAppearance_action();
		}

		void adjustSizes()
		{
			m_text.setSize({ {0,0}, {100,100} });
			updateSurfaceSize(m_text);
			prepareText();
			if (m_display_scroll)
			{
				if (getNumberOfLines() > m_text.getSize().y)
				{
					m_text.setSize({ {-1,0}, {100,100} });
					updateSurfaceSize(m_text);
					prepareText();
				}
			}
			m_scroll.setContentLength(getNumberOfLines());
		}

		void setText(console_string txt)
		{
			m_unprepared_text = txt;
			adjustSizes();
			fill();
		}

		console_string getText() { return m_unprepared_text; }
		int getNumberOfLines()
		{
			return ceil(m_prepared_text.size() / (m_text.getSize().x * 1.f));
		}

		void goToLine(int line)
		{
			m_scroll.setHandlePosition(line);
			fill();
		}

		void lineUp()
		{
			m_scroll.setHandlePosition(m_scroll.getHandlePosition() - 1);
			fill();
		}
		void lineDown()
		{
			m_scroll.setHandlePosition(m_scroll.getHandlePosition() + 1);
			fill();
		}
		void pageUp()
		{
			m_scroll.setHandlePosition(m_scroll.getHandlePosition() - getSize().y);
			fill();
		}
		void pageDown()
		{
			m_scroll.setHandlePosition(m_scroll.getHandlePosition() + getSize().y);
			fill();
		}

		void usePreparedText(bool use)
		{
			m_use_prepared_text = use;
			adjustSizes();
			fill();
		}
		bool isUsingPreparedText() { return m_use_prepared_text; }

		void useDensePunctuation(bool use)
		{
			m_use_dense_punctuation = use;
			adjustSizes();
			fill();
		}
		bool isUsingDensePunctuation() { return m_use_dense_punctuation; }

		void useControlCharacters(bool use)
		{
			m_use_control_characters = use;
			adjustSizes();
			fill();
		}
		bool isUsingControlCharacters() { return m_use_control_characters; }

		void displayScroll(bool display)
		{
			m_display_scroll = display;
			adjustSizes();
			fill();
		}
		bool isDisplayingScroll() { return m_display_scroll; }

		void resizeToText(int max_width)
		{
			if (max_width <= 0)
			{
				m_use_control_characters = false;//temporarily disable control characters

				setSize({ {m_unprepared_text.size(), 1} });
				adjustSizes();
				/*
				with control characters disabled one line prepared text could be shorter than unprepared text
				e.g. space at start of line will be removed and all control characters will be removed
				*/
				setSize({ {m_prepared_text.size(), 1} });

				m_use_control_characters = true;
			}
			else
			{
				m_display_scroll = false;

				setSize({ {max_width, 1} });
				prepareText();

				setSize({ {max_width, (int)ceil(1.f * m_prepared_text.size() / max_width)} });
				fill();

				m_display_scroll = true;
			}
		}
		void resizeToText() { resizeToText(0); }

		void update()
		{
			if (isActive()) {
				if (KEYBOARD::isKeyPressed(keyUp)) {
					lineUp();
				}
				if (KEYBOARD::isKeyPressed(keyDown)) {
					lineDown();
				}
				if (KEYBOARD::isKeyPressed(keyPageUp)) {
					pageUp();
				}
				if (KEYBOARD::isKeyPressed(keyPageDown)) {
					pageDown();
				}
			}
		}


		void draw_action()
		{
			update();
		}
		void resize_action()
		{
			//m_scroll.setVisibleContentLength(getSize().y);
			updateSurfaceSize(m_scroll);
			adjustSizes();
			fill();
		}

		void activation_action()
		{
			m_scroll.setAppearance(m_active_scroll);
			if (m_display_scroll && m_scroll.isNeeded()) { insertSurface(m_scroll); }
		}
		void disactivation_action()
		{
			m_scroll.setAppearance(m_inactive_scroll);
			if (m_display_scroll && m_scroll.isNeeded()) { insertSurface(m_scroll); }
		}
		void setAppearance_action() override
		{
			if (isActive()) { m_scroll.setAppearance(m_active_scroll); }
			else { m_scroll.setAppearance(m_inactive_scroll); }
			if (m_display_scroll && m_scroll.isNeeded()) { insertSurface(m_scroll); }
		}

	};
}