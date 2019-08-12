#pragma once
#include "tui_console.h"
#include "tui_appearance.h"
#include "tui_text_utils.h"

namespace tui
{
	struct button_appearance_a
	{
		symbol first;
		symbol last;
		button_appearance_a() : button_appearance_a('[',']') {}
		button_appearance_a(symbol f, symbol l) : first(f), last(l) {}

		void setColor(color Color)
		{
			first.setColor(Color);
			last.setColor(Color);
		}
	};

	struct button_appearance : appearance
	{
	protected:
		button_appearance_a m_active_appearance;
		button_appearance_a m_inactive_appearance;
	public:
		button_appearance() : button_appearance({ { '[', COLOR::WHITE }, { ']', COLOR::WHITE } },
												{ { '[', COLOR::DARKGRAY }, { ']', COLOR::DARKGRAY } } ) {}
		button_appearance(button_appearance_a active, button_appearance_a inactive) : m_active_appearance(active), m_inactive_appearance(inactive) {}

		void setColor(color Color) override
		{
			m_active_appearance.setColor(Color);
			m_inactive_appearance.setColor(Color);
			setAppearance_action();
		}

		void setAppearance(button_appearance appearance)
		{
			*this = appearance;
			setAppearance_action();
		}
		button_appearance getAppearance() { return *this; }

		void setActiveAppearance(button_appearance_a active)
		{
			m_active_appearance = active;
			setAppearance_action();
		}
		button_appearance_a getActiveAppearance() { return m_active_appearance; }

		void setInactiveAppearance(button_appearance_a inactive)
		{
			m_inactive_appearance = inactive;
			setAppearance_action();
		}
		button_appearance_a getInactiveAppearance() { return m_inactive_appearance; }
	};

	template<int direction>
	struct button : surface1D<direction>, button_appearance, active_element
	{
	private:
		console_string m_selected_text;
		console_string m_deselected_text;

		bool m_selected = false;

		bool m_redraw_needed = true;

		button_appearance_a getCurrentAppearance()
		{
			if (isActive()) { return m_active_appearance; }
			else { return m_inactive_appearance; }
		}
		const console_string& getCurrentText()
		{
			if (isSelected()) { return m_selected_text; }
			else { return m_deselected_text; }
		}

		void fill()
		{
			surface::makeBlank();

			surface1D<direction>::setSymbolAt(getCurrentAppearance().first, 0);
			for (int i = 1;i<surface1D<direction>::getSize() && i - 1 < getCurrentText().size(); i++)
			{
				surface1D<direction>::setSymbolAt(getCurrentText()[i-1], i);
			}
			surface1D<direction>::setSymbolAt(getCurrentAppearance().last, surface1D<direction>::getSize()-1);

			if (isSelected()) { surface::invert(); }
		}

	public:
		int keySelect = KEYBOARD::KEY::ENTER;

		button(surface1D_size size, console_string selected, console_string deselected) : m_selected_text(selected), m_deselected_text(deselected)
		{
			surface1D<direction>::setSize(size);
		}
	
		bool isSelected() { return m_selected; }

		void update()
		{
			if (isActive())
			{
				if (KEYBOARD::isKeyPressed(keySelect))
				{
					m_selected = !m_selected;
					m_redraw_needed = true;
				}
			}
		}

		void draw_action() override 
		{ 
			update(); 
			if (m_redraw_needed)
			{
				fill();
				m_redraw_needed = false;
			}
		}

		void resize_action() override { m_redraw_needed = true; }
		void setAppearance_action() override { m_redraw_needed = true; }
	};

}