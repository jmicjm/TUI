#pragma once
#include "tui_surface.h"
#include "tui_appearance.h"
#include "tui_text_utils.h"
#include "tui_active_element.h"

namespace tui
{
	struct radio_button_appearance_a
	{
		symbol selected;
		symbol not_selected;

#ifdef TUI_TARGET_SYSTEM_WINDOWS
		radio_button_appearance_a() : radio_button_appearance_a(U'\x25CF', U'\x25CB') {}
#endif
#ifdef TUI_TARGET_SYSTEM_LINUX
		radio_button_appearance_a() : radio_button_appearance_a(U'\x25C9', U'\x25CB') {}
#endif
		radio_button_appearance_a(symbol Selected, symbol Not_selected) : selected(Selected), not_selected(Not_selected) {}

		void setColor(color Color)
		{
			selected.setColor(Color);
			not_selected.setColor(Color);
		}
	};

	struct radio_button_appearance : appearance
	{
	protected:
		radio_button_appearance_a active_appearance;
		radio_button_appearance_a inactive_appearance;
	public:
		radio_button_appearance()
		{
			active_appearance.setColor(COLOR::WHITE);
			inactive_appearance.setColor(COLOR::DARKGRAY);
		}

		void setColor(color Color) override
		{
			active_appearance.setColor(Color);
			inactive_appearance.setColor(Color);
			setAppearanceAction();
		}

		void setAppearance(radio_button_appearance appearance) { setElement(*this, appearance); }
		radio_button_appearance getAppearance() { return *this; }

		void setActiveAppearance(radio_button_appearance_a active) { setElement(active_appearance, active); }
		radio_button_appearance_a getActiveAppearance() { return active_appearance; }

		void setInactiveAppearance(radio_button_appearance_a inactive) { setElement(inactive_appearance, inactive); }
		radio_button_appearance_a getInactiveAppearance() { return inactive_appearance; }
	};

	template<DIRECTION direction>
	struct radio_button : surface1D<direction>, radio_button_appearance, active_element
	{
	private:
		unsigned int m_options = 0;
		unsigned int m_selected_option = 0;

		bool m_redraw_needed = true;

		radio_button_appearance_a getCurrentAppearance()
		{
			if (isActive()) { return active_appearance; }
			else { return inactive_appearance; }
		}

		void fill()
		{
			surface1D<direction>::makeTransparent();

			for (int i = 0; i < m_options && i < surface1D<direction>::getSize(); i++)
			{
				if (i == m_selected_option)
				{
					surface1D<direction>::setSymbolAt(getCurrentAppearance().selected, i);
				}
				else
				{
					surface1D<direction>::setSymbolAt(getCurrentAppearance().not_selected, i);
				}
			}
		}

		void resizeAction() override { m_redraw_needed = true; }
		void updateAction() override { update(); }
		void drawAction() override
		{
			if(m_redraw_needed)
			{
				fill();
			}
			m_redraw_needed = false;
		}

		void setAppearanceAction() override { m_redraw_needed = true; }
		void activationAction() override { m_redraw_needed = true; }
		void disactivationAction() override { m_redraw_needed = true; }

	public:
		short key_next = input::KEY::DOWN;
		short key_previous = input::KEY::UP;

		radio_button()
		{
			if (direction == DIRECTION::HORIZONTAL)
			{
				key_next = input::KEY::RIGHT;
				key_previous = input::KEY::LEFT;
			}
		}

		void setOptionsAmount(unsigned int amount) 
		{
			m_options = amount;
			m_redraw_needed = true;
		}
		unsigned int getOptionsAmount() { return m_options; }

		void setSelectedOption(unsigned int option)
		{
			if (option <= m_options)
			{
				m_selected_option = option;
			}
			m_redraw_needed = true;
		}
		unsigned int getSelectedOption() { return m_selected_option; }

		void nextOption()
		{
			if (m_selected_option < m_options - 1)
			{
				m_selected_option++;
			}
			else
			{
				m_selected_option = 0;
			}
			m_redraw_needed = true;
		}
		void previousOption()
		{
			if (m_selected_option > 0)
			{
				m_selected_option--;
			}
			else
			{
				m_selected_option = m_options -1;
			}
			m_redraw_needed = true;
		}

		void resizeToOptions()
		{
			surface1D<direction>::setSizeInfo({ {(int)m_options} });
		}

		void update()
		{
			if (isActive())
			{
				if (input::isKeyPressed(key_next)) { nextOption(); }
				if (input::isKeyPressed(key_previous)) { previousOption(); }
			}
		}
	};
}
