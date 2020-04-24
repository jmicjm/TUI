#pragma once
#include "tui_surface.h"
#include "tui_appearance.h"
#include "tui_text_utils.h"
#include "tui_active_element.h"
#include "tui_input.h"

#include <vector>

namespace tui
{
	struct tabs_appearance_a
	{
		symbol separator;
		symbol prev_arrow;
		symbol next_arrow;
	public:
		tabs_appearance_a(DIRECTION direction = DIRECTION::HORIZONTAL)
		{
			switch (direction)
			{
			case DIRECTION::HORIZONTAL:
				separator = '|';
				prev_arrow = U'\x25C4';
				next_arrow = U'\x25BA';
				break;
			case DIRECTION::VERTICAL:
				separator = '-';
				prev_arrow = U'\x25B2';
				next_arrow = U'\x25BC';
			}
		}

		void setColor(color Color)
		{
			separator.setColor(Color);
			prev_arrow.setColor(Color);
			next_arrow.setColor(Color);
		}
	};

	struct tabs_appearance : appearance
	{
	protected:
		tabs_appearance_a active_appearance;
		tabs_appearance_a inactive_appearance;
	public:
		tabs_appearance(DIRECTION direction = DIRECTION::HORIZONTAL) : active_appearance(direction), inactive_appearance(direction)
		{
			inactive_appearance.setColor(COLOR::DARKGRAY);
		}

		void setColor(color Color) override
		{
			active_appearance.setColor(Color);
			inactive_appearance.setColor(Color);
			setAppearanceAction();
		}

		void setAppearance(tabs_appearance appearance) { setElement(*this, appearance); }
		tabs_appearance getAppearance() { return *this; }

		void setActiveAppearance(tabs_appearance_a appearance) { setElement(active_appearance, appearance); }
		tabs_appearance_a getActiveAppearance() { return active_appearance; }

		void setInactiveAppearance(tabs_appearance_a appearance) { setElement(inactive_appearance, appearance); }
		tabs_appearance_a getInactiveAppearance() { return inactive_appearance; }
	};

	template<DIRECTION direction>
	struct tabs : surface1D<direction>, tabs_appearance, active_element
	{
	private:
		struct tab_info
		{
			console_string string;
			unsigned int position = 0;
		};

		std::vector<tab_info> m_tabs;
		console_string m_generated_tabs;
		unsigned int m_selected = 0;
		unsigned int m_first_pos = 0;

		bool m_display_separator = true;
		bool m_redraw_needed = true;

		tabs_appearance_a getCurrentAppearance()
		{
			if (isActive()) { return active_appearance; }
			else { return inactive_appearance; }
		}

		void generateTabs()
		{
			m_generated_tabs.clear();

			for (int i = 0; i < m_tabs.size(); i++)
			{
				m_tabs[i].position = m_generated_tabs.size();
				m_generated_tabs += m_tabs[i].string;

				if (m_display_separator && i < m_tabs.size() - 1) 
				{
					m_generated_tabs += getCurrentAppearance().separator;
				}
			}
		}

		void fill()
		{
			if (m_tabs.size() > 0)
			{
				bool too_long = m_generated_tabs.size() > surface1D<direction>::getSize();
				if (too_long)
				{
					surface1D<direction>::setSymbolAt(getCurrentAppearance().prev_arrow, 0);
					surface1D<direction>::setSymbolAt(getCurrentAppearance().next_arrow, surface1D<direction>::getSize() - 1);

					if (m_tabs[m_selected].position < m_first_pos)
					{
						m_first_pos = m_tabs[m_selected].position;
					}
					else if (m_tabs[m_selected].position + m_tabs[m_selected].string.size() >= surface1D<direction>::getSize() - 2 * too_long)
					{
						m_first_pos = m_tabs[m_selected].position + m_tabs[m_selected].string.size() - (surface1D<direction>::getSize() - 2 * too_long);
					}
				}
				else 
				{
					surface1D<direction>::makeTransparent();
					m_first_pos = 0; 
				}

				auto strIdx = [=](int i)
				{
					return i - too_long + m_first_pos;
				};

				for (int i = too_long; strIdx(i) < m_generated_tabs.size() && i < surface1D<direction>::getSize() - too_long; i++)
				{
					switch (strIdx(i) >= m_tabs[m_selected].position && strIdx(i) < m_tabs[m_selected].position + m_tabs[m_selected].string.size())
					{
					case false:
						surface1D<direction>::setSymbolAt(m_generated_tabs[strIdx(i)], i);
						break;
					case true:
						symbol s = m_generated_tabs[strIdx(i)];
						s.invert();
						surface1D<direction>::setSymbolAt(s, i);
					}
				}
			}
		}

		void resizeAction() override { m_redraw_needed = true; }
		void updateAction() override { update(); }
		void setAppearanceAction() override 
		{
			generateTabs();
			m_redraw_needed = true; 
		}
		void drawAction() override
		{
			if (m_redraw_needed) { fill(); }
			m_redraw_needed = false;
		}
		void activationAction()
		{
			generateTabs();
			m_redraw_needed = true;
		}
	public:
		short key_prev = input::KEY::LEFT;
		short key_next = input::KEY::RIGHT;

		tabs() : tabs_appearance(direction) 
		{
			if (direction == tui::DIRECTION::VERTICAL)
			{
				key_prev = input::KEY::UP;
				key_next = input::KEY::DOWN;
			}
		}

		void setTabs(const std::vector<console_string>& tabs)
		{
			m_tabs.resize(tabs.size());
			for (int i = 0; i < m_tabs.size(); i++)
			{
				m_tabs[i].string = tabs[i];
			}
			generateTabs();
			m_redraw_needed = true;
		}

		void resizeToTabs()
		{
			surface1D<direction>::setSizeInfo({ m_generated_tabs.size() });
			m_redraw_needed = true;
		}

		unsigned int getTabCount() { return m_tabs.size(); }
		unsigned int getSelected() { return m_selected; }
		void setSelected(unsigned int s)
		{
			if (m_selected != s)
			{
				m_selected = s;
				m_redraw_needed = true;
			}
		}

		void displaySeparator(bool use)
		{
			if (m_display_separator != use) 
			{
				m_display_separator = use;
				generateTabs();
				m_redraw_needed = true; 
			}
		}
		bool isDiplayingSeparator() { return m_display_separator; }

		void nextTab()
		{
			if (m_selected < m_tabs.size() - 1)
			{
				m_selected++;
			}
			else { m_selected = 0; }
			m_redraw_needed = true;
		}
		void prevTab()
		{
			if (m_selected > 0)
			{
				m_selected--;
			}
			else { m_selected = m_tabs.size() - 1; }
			m_redraw_needed = true;
		}

		void update()
		{
			if (isActive())
			{
				if (input::isKeyPressed(key_prev)) { prevTab(); }
				if (input::isKeyPressed(key_next)) { nextTab(); }
			}
		}
	};
}
