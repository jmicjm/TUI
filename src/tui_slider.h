/*this file contains following elements:
struct slider_appearance_a - describes active/inactive slider appearance, used by slider_appearance
struct slider_appearance - describes slider appearance
struct slider_button - widget that displays a slider*/
#pragma once
#include "tui_surface.h"
#include "tui_appearance.h"
#include "tui_active_element.h"
#include "tui_text_utils.h"
#include "tui_input.h"

namespace tui
{
	struct slider_appearance_a
	{
		symbol slider;
		symbol line;
		color value_color;
		color min_color;
		color max_color;
		color percentage_color;

		slider_appearance_a(DIRECTION direction = DIRECTION::HORIZONTAL)
		{
			switch (direction)
			{
			case DIRECTION::HORIZONTAL:
				*this = slider_appearance_a(U'\x2550', U'\x2500');
				break;
			case DIRECTION::VERTICAL:
				*this = slider_appearance_a(U'\x2551', U'\x2502');
			}
		}
		slider_appearance_a(symbol slider, symbol line) : slider(slider), line(line) {}

		void setColor(color Color)
		{
			slider.setColor(Color);
			line.setColor(Color);
			value_color = Color;
			min_color = Color;
			max_color = Color;
			percentage_color = Color;
		}
	};

	struct slider_appearance : appearance
	{
	protected:
		slider_appearance_a active_appearance;
		slider_appearance_a inactive_appearance;
	public:
		slider_appearance(DIRECTION direction = DIRECTION::HORIZONTAL) : active_appearance(direction), inactive_appearance(direction) 
		{
			inactive_appearance.setColor(COLOR::DARKGRAY);
		}
		slider_appearance(slider_appearance_a active, slider_appearance_a inactive) : active_appearance(active), inactive_appearance(inactive) {}

		void setColor(color Color) override
		{
			active_appearance.setColor(Color);
			inactive_appearance.setColor(Color);
			setAppearanceAction();
		}

		void setAppearance(slider_appearance appearance) { setElement(*this, appearance); }
		slider_appearance getAppearance() const { return *this; }

		void setActiveAppearance(slider_appearance_a active) { setElement(active_appearance, active); }
		slider_appearance_a getActiveAppearance() const { return active_appearance; }

		void setInactiveAppearance(slider_appearance_a inactive) { setElement(inactive_appearance, inactive); }
		slider_appearance_a getInactiveAppearance() const { return inactive_appearance; }
	};

	template<DIRECTION direction>
	struct slider : surface1D<direction>, active_element, slider_appearance
	{
	private:
		float m_min;
		float m_max;
		float m_value;

		float m_step = 0.1;
		bool m_use_absolute_step = false;

		bool m_display_value_label = false;
		bool m_display_min_label = false;
		bool m_display_max_label = false;
		bool m_display_percentage_label = false;
		bool m_display_labels_at_end = true;
		int m_labels_precision = -1;

		bool m_redraw_needed = true;

		slider_appearance_a gca() const
		{
			if (isActive()) { return active_appearance; }
			else { return inactive_appearance; }
		}

		symbol_string getValStr() const
		{
			symbol_string val_str;

			if (m_display_percentage_label)
			{
				float val;

				switch (m_value > 0)
				{
				case true:
					val = m_value / m_max;
					break;
				case false:
					if (m_min != 0)
					{
						val = m_value / m_min * -1;
					}
					else
					{
						val = 0;
					}
				}

				val_str += {toStringP(val * 100, m_labels_precision) + '%', gca().percentage_color};
			}
			if (m_display_min_label) { val_str += {toStringP(m_min, m_labels_precision) + '/', gca().min_color}; }
			if (m_display_value_label) { val_str += {toStringP(m_value, m_labels_precision), gca().value_color}; }
			if (m_display_max_label) { val_str += {'/' + toStringP(m_max, m_labels_precision), gca().max_color}; }

			return val_str;
		}

		void fill()
		{
			symbol_string val_str = getValStr();
			
			int slider_size = surface1D<direction>::getSize() - val_str.size();
			slider_size = slider_size >= 0 ? slider_size : 0;

			unsigned int slider_offset = val_str.size() * !m_display_labels_at_end;
			unsigned int str_offset = slider_size * m_display_labels_at_end;

			float distance = fabs(m_min - m_max);
			float min_distance = fabs(m_min - m_value);
			float slider_perc_pos = min_distance / distance;
			unsigned int slider_pos = slider_perc_pos * (slider_size - 1);

			if (direction == DIRECTION::VERTICAL)
			{
				slider_pos = (slider_size - 1) - slider_pos;
			}

			for (int i = 0; i < slider_size && i + slider_offset < surface1D<direction>::getSize(); i++)
			{
				if (i == slider_pos)
				{
					surface1D<direction>::setSymbolAt(gca().slider, i + slider_offset);
				}
				else
				{
					surface1D<direction>::setSymbolAt(gca().line, i + slider_offset);
				}
			}

			for (int i = 0; i < val_str.size() && i + str_offset < surface1D<direction>::getSize(); i++)
			{
				surface1D<direction>::setSymbolAt(val_str[i], i + str_offset);
			}
			
		}

		template<typename T>
		void setProperty(T& elem_to_set, T elem)
		{
			elem_to_set = elem;
			m_redraw_needed = true;
		}

		void resizeAction() override { m_redraw_needed = true; }
		void updateAction() override { update(); }
		void drawAction(surface::action_proxy proxy) override
		{
			if (m_redraw_needed) { fill(); }
			m_redraw_needed = false;
		};

		void activationAction() override { m_redraw_needed = true; }
		void deactivationAction() override { m_redraw_needed = true; }

		void setAppearanceAction() override { m_redraw_needed = true; }
	public:
		short key_increase = input::KEY::RIGHT;
		short key_decrease = input::KEY::LEFT;

		slider(surface1D_size size = surface1D_size(), float min = 0, float max = 0, float value = 0)
			: m_min(min), m_max(max), m_value(0), slider_appearance(direction)
		{
			setMinValue(min);
			setMaxValue(max);
			setValue(value);

			if (direction == DIRECTION::VERTICAL)
			{
				key_increase = input::KEY::UP;
				key_decrease = input::KEY::DOWN;
			}
			surface1D<direction>::setSizeInfo(size);
		}

		void setMaxValue(float max)
		{
			setProperty(m_max, std::max(max, m_min));
		}
		float getMaxValue() const { return m_max; }

		void setMinValue(float min)
		{
			setProperty(m_min, std::min(min, m_max));
		}
		float getMinValue() const { return m_min; }

		void setValue(float value)
		{
			value = std::max(value, m_min);
			value = std::min(value, m_max);
			setProperty(m_value, value);
		}
		float getValue() const { return m_value; }

		void displayLabels(bool display)
		{
			m_display_value_label = display;
			m_display_min_label = display;
			m_display_max_label = display;
			m_display_percentage_label = display;

			m_redraw_needed = true;
		}
		bool isDisplayingLabels() const
		{
			return m_display_value_label || m_display_min_label || m_display_max_label || m_display_percentage_label;
		}

		void displayValueLabel(bool display) { setProperty(m_display_value_label, display); }
		bool isDisplayingValueLabel() const { return m_display_value_label; }

		void displayMinLabel(bool display) { setProperty(m_display_min_label, display); }
		bool isDisplayingMinLabel() const { return m_display_min_label; }

		void displayMaxLabel(bool display) { setProperty(m_display_max_label, display); }
		bool isDisplayingMaxLabel() const { return m_display_max_label; }

		void displayPercentageLabel(bool display) { setProperty(m_display_percentage_label, display); }
		bool isDisplayingPercentageLabel() const { return m_display_percentage_label; }

		void displayLabelsAtEnd(bool display) { setProperty(m_display_labels_at_end, display); }
		bool isDisplayingLabelsAtEnd() const { return m_display_labels_at_end; }

		void setLabelsPrecision(int precision) { setProperty(m_labels_precision, precision); }
		int getLabelsPrecision() const { return m_labels_precision; }

		void useAbsoluteStep(bool use) { m_use_absolute_step = use; }
		bool isUsingAbsoluteStep() const { return m_use_absolute_step; }

		void setStepValue(float step) { m_step = step; }
		float getStepValue() const { return m_step; }

		void stepUp()
		{
			switch (m_use_absolute_step)
			{
			case false:
				setValue(m_value + m_step * abs(m_min - m_max));
				break;
			case true:
				setValue(m_value + m_step);
			}
		}
		void stepDown()
		{
			switch (m_use_absolute_step)
			{
			case false:
				setValue(m_value - m_step * abs(m_min - m_max));
				break;
			case true:
				setValue(m_value - m_step);
			}
		}

		void update()
		{
			if (isActive())
			{
				if (input::isKeyPressed(key_increase))
				{
					stepUp();
				}
				if (input::isKeyPressed(key_decrease))
				{
					stepDown();
				}		
			}
		}
	};
}