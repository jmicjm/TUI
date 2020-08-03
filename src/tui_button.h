#pragma once
#include "tui_surface.h"
#include "tui_appearance.h"
#include "tui_text_utils.h"
#include "tui_active_element.h"

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
			setAppearanceAction();
		}

		void setAppearance(button_appearance appearance) { setElement(*this, appearance); }
		button_appearance getAppearance() { return *this; }

		void setActiveAppearance(button_appearance_a active) { setElement(m_active_appearance, active); }
		button_appearance_a getActiveAppearance() { return m_active_appearance; }

		void setInactiveAppearance(button_appearance_a inactive) { setElement(m_inactive_appearance, inactive); }
		button_appearance_a getInactiveAppearance() { return m_inactive_appearance; }
	};
	namespace BUTTON_TYPE
	{
		enum BUTTON_TYPE {SWITCH, PUSH};
	}

	template<DIRECTION direction>
	struct button : surface1D<direction>, button_appearance, active_element
	{
	private:
		symbol_string m_selected_text;
		symbol_string m_deselected_text;

		void (*m_selection_function_ptr)(void) = nullptr;
		void (*m_deselection_function_ptr)(void) = nullptr;

		bool m_selected = false;

		bool m_redraw_needed = true;

		button_appearance_a getCurrentAppearance()
		{
			if (isActive()) { return m_active_appearance; }
			else { return m_inactive_appearance; }
		}
		const symbol_string& getCurrentText()
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
		short key_select = input::KEY::ENTER;
		BUTTON_TYPE::BUTTON_TYPE type = BUTTON_TYPE::PUSH;

		button() : button({3}, ' ') {}
		button(surface1D_size size, symbol_string text) : button(size, text, text) {}
		button(surface1D_size size, symbol_string selected, symbol_string deselected) : button(size, selected, deselected, button_appearance()) {}
		button(surface1D_size size, symbol_string selected, symbol_string deselected, button_appearance appearance) : m_selected_text(selected), m_deselected_text(deselected)
		{
			surface1D<direction>::setSize(size);
			setAppearance(appearance);
		}
	
		bool isSelected() { return m_selected; }

		void setSelectedText(symbol_string text)
		{
			m_selected_text = text;
			if (isSelected()) { m_redraw_needed = true; }
		}
		void setDeselectedText(symbol_string text)
		{
			m_deselected_text = text;
			if (!isSelected()) { m_redraw_needed = true; }
		}
		void setText(symbol_string text)
		{
			m_selected_text = text;
			m_deselected_text = text;
			m_redraw_needed = true;
		}

		void resizeToText()
		{
			unsigned int longest = m_selected_text.size() > m_deselected_text.size() ? m_selected_text.size() : m_deselected_text.size();
			surface1D<direction>::setSize(longest+2);
		}

		void update()
		{
			bool last_state = isSelected();

			if (type == BUTTON_TYPE::PUSH) { m_selected = false; }

			if (isActive())
			{
				if (input::isKeyPressed(key_select))
				{
					m_selected = !m_selected;

					auto launchFunc = [](void (*func_ptr)(void))
					{
						if (func_ptr != nullptr) { (*func_ptr)(); }
					};

					switch (isSelected())
					{
					case true:
						launchFunc(m_selection_function_ptr);
						break;
					case false:
						launchFunc(m_deselection_function_ptr);
					}
				}
			}

			if (last_state != isSelected()) { m_redraw_needed = true; }
		}

		void setSelectionFunction(void (*func_ptr)(void))
		{
			m_selection_function_ptr = *func_ptr;
		}

		void setDeselectionFunction(void (*func_ptr)(void))
		{
			m_deselection_function_ptr = *func_ptr;
		}

		void updateAction() override { update(); }

		void drawAction() override 
		{ 
			if (m_redraw_needed)
			{
				fill();
				m_redraw_needed = false;
			}
		}

		void resizeAction() override { m_redraw_needed = true; }
		void setAppearanceAction() override { m_redraw_needed = true; }

		void activationAction() override { m_redraw_needed = true; }
		void disactivationAction() override { m_redraw_needed = true; }
	};

}