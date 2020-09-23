/*this file contains following elements:
struct button_appearance_a - describes active/inactive button appearance, used by button_appearance
struct button_appearance - describes button appearance
struct button<tui::DIRECTION> - widget that displays a button*/
#pragma once
#include "tui_surface.h"
#include "tui_appearance.h"
#include "tui_text_utils.h"
#include "tui_active_element.h"
#include "tui_input.h"

#include <algorithm>
#include <functional>

namespace tui
{
	struct button_appearance_a
	{
		symbol first;
		symbol first_selected;
		symbol last;
		symbol last_selected;
		button_appearance_a() : button_appearance_a('[','<',']','>') {}
		button_appearance_a(symbol f, symbol f_sel, symbol l, symbol l_sel) : first(f), first_selected(f_sel), last(l), last_selected(l_sel) {}

		void setColor(color Color)
		{
			first.setColor(Color);
			first_selected.setColor(Color);
			last.setColor(Color);
			last_selected.setColor(Color);
		}
	};

	struct button_appearance : appearance
	{
	protected:
		button_appearance_a active_appearance;
		button_appearance_a inactive_appearance;
	public:
		button_appearance() {}
		button_appearance(button_appearance_a active, button_appearance_a inactive) : active_appearance(active), inactive_appearance(inactive) {}

		void setColor(color Color) override
		{
			active_appearance.setColor(Color);
			inactive_appearance.setColor(Color);
			setAppearanceAction();
		}

		void setAppearance(button_appearance appearance) { setElement(*this, appearance); }
		button_appearance getAppearance() const { return *this; }

		void setActiveAppearance(button_appearance_a active) { setElement(active_appearance, active); }
		button_appearance_a getActiveAppearance() const { return active_appearance; }

		void setInactiveAppearance(button_appearance_a inactive) { setElement(inactive_appearance, inactive); }
		button_appearance_a getInactiveAppearance() const { return inactive_appearance; }
	};

	enum class BUTTON_TYPE { SWITCH, PUSH };

	template<DIRECTION direction>
	struct button : surface1D<direction>, button_appearance, active_element
	{
	private:
		symbol_string m_selected_text;
		symbol_string m_deselected_text;

		BUTTON_TYPE m_type = BUTTON_TYPE::PUSH;

		std::function<void()> select_function = nullptr;
		std::function<void()> deselect_function = nullptr;

		bool m_selected = false;

		bool m_redraw_needed = true;

		button_appearance_a gca() const
		{
			if (isActive()) { return active_appearance; }
			else { return inactive_appearance; }
		}
		symbol_string gct() const
		{
			if (isSelected()) { return getFullWidthString(m_selected_text); }
			else { return getFullWidthString(m_deselected_text); }
		}

		void fill()
		{
			surface::makeBlank();

			for (int i = 1;i<surface1D<direction>::getSize() && i - 1 < gct().size(); i++)
			{
				surface1D<direction>::setSymbolAt(gct()[i-1], i);
			}

			switch (isSelected())
			{
			case false:
				surface1D<direction>::setSymbolAt(gca().first, 0);
				surface1D<direction>::setSymbolAt(gca().last, surface1D<direction>::getSize() - 1);
				break;
			case true:
				surface1D<direction>::setSymbolAt(gca().first_selected, 0);
				surface1D<direction>::setSymbolAt(gca().last_selected, surface1D<direction>::getSize() - 1);
			}

			if (isActive()) { surface::invert(); }
		}

		void resizeAction() override { m_redraw_needed = true; }
		void updateAction() override { update(); }
		void drawAction(surface::action_proxy proxy) override
		{
			if (m_redraw_needed)
			{
				fill();
				m_redraw_needed = false;
			}
		}
		void activationAction() override { m_redraw_needed = true; }
		void deactivationAction() override { m_redraw_needed = true; }

		void setAppearanceAction() override { m_redraw_needed = true; }

	public:
		short key_select = input::KEY::ENTER;

		button() : button(' ') {}
		button(const symbol_string& text) : button(text, text) {}
		button(const symbol_string& selected, const symbol_string& deselected) : m_selected_text(selected), m_deselected_text(deselected)
		{
			resizeToText();
		}
		button(surface1D_size size, const symbol_string& text) : button(size, text, text) {}
		button(surface1D_size size, const symbol_string& selected, const symbol_string& deselected) : m_selected_text(selected), m_deselected_text(deselected)
		{
			surface1D<direction>::setSizeInfo(size);
		}

		void setType(BUTTON_TYPE type) { m_type = type; }
		BUTTON_TYPE getType() { return m_type; }
	
		bool isSelected() const { return m_selected; }

		void setSelectedText(const symbol_string& text)
		{
			m_selected_text = text;
			if (isSelected()) { m_redraw_needed = true; }
		}
		symbol_string getSelectedText() { return m_selected_text; }

		void setDeselectedText(const symbol_string& text)
		{
			m_deselected_text = text;
			if (!isSelected()) { m_redraw_needed = true; }
		}
		symbol_string getDeselectedText() { return m_deselected_text; }

		void setText(const symbol_string& text)
		{
			m_selected_text = text;
			m_deselected_text = text;
			m_redraw_needed = true;
		}

		void resizeToText()
		{
			unsigned int s_size = getFullWidthString(m_selected_text).size();
			unsigned int d_size = getFullWidthString(m_deselected_text).size();

			unsigned int longest = std::max(s_size, d_size);

			surface1D<direction>::setSizeInfo(longest+2);
		}

		void setSelectFunction(std::function<void()> func)
		{
			select_function = func;
		}

		void setDeselectFunction(std::function<void()> func)
		{
			deselect_function = func;
		}

		void update()
		{
			bool last_state = isSelected();

			if (m_type == BUTTON_TYPE::PUSH) { m_selected = false; }

			if (isActive())
			{
				if (input::isKeyPressed(key_select))
				{
					m_selected = !m_selected;

					switch (isSelected())
					{
					case true:
						if (select_function) { select_function(); }
						break;
					case false:
						if (deselect_function) { deselect_function(); }
					}
				}
			}

			if (last_state != isSelected()) { m_redraw_needed = true; }
		}
	};

}