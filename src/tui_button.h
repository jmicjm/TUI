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
		console_string m_active_text;
		console_string m_inactive_text;

		button_appearance_a getCurrentAppearance()
		{
			if (isActive()) { return m_active_appearance; }
			else { return m_inactive_appearance; }
		}

		void fill()
		{
			//surface1D<direction>::setSymbol(getCurrentAppearance().first, 0);
		}

	public:



		void draw_action() override { fill(); }
	};

}