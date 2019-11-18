#pragma once
#include "tui_surface.h"
#include "tui_appearance.h"
#include "tui_text_utils.h"

namespace tui
{
	struct bar_appearance : appearance
	{
	protected:
		symbol full;
		symbol empty;
		symbol half;
		color value_color;
		color min_color;
		color max_color;
		color percentage_color;
	public:
		bar_appearance(bool direction = tui::DIRECTION::VERTICAL) : bar_appearance({ U'\x2588', COLOR::WHITE }, { U' ', {COLOR::DARKGRAY, COLOR::DARKGRAY} })
		{
			switch (direction)
			{
			case tui::DIRECTION::VERTICAL:
				half = { U'\x2584', {COLOR::WHITE, COLOR::DARKGRAY} };
				break;
			case tui::DIRECTION::HORIZONTAL:
				half = { U'\x258C', {COLOR::WHITE, COLOR::DARKGRAY} };
				break;
			}
		}
		bar_appearance(symbol Full, symbol Empty) : bar_appearance(Full, Empty, Full) {}
		bar_appearance(symbol Full, symbol Empty, symbol Half) : full(Full), empty(Empty), half(Half) {}

		void setColor(color Color) override
		{
			full.setColor(Color);
			empty.setColor(Color);
			half.setColor(Color);
			value_color = Color;
			min_color = Color;
			max_color = Color;
			percentage_color = Color;

			setAppearanceAction();
		}

		void setAppearance(bar_appearance appearance) { setElement(*this, appearance); }
		bar_appearance getAppearance() { return *this; }

		void setFullSymbol(symbol Full) { setElement(full, Full); }
		symbol getFullSymbol() { return full; }

		void setEmptySymbol(symbol Empty) { setElement(empty, Empty); }
		symbol getEmptySymbol() { return empty; }

		void setHalfSymbol(symbol Half) { setElement(half, Half); }
		symbol getHalfSymbol() { return half; }

		void setValueColor(color Color) { setElement(value_color, Color); }
		color getValueColor() { return value_color; }

		void setMinColor(color Color) { setElement(min_color, Color); }
		color getMinColor() { return min_color; }

		void setMaxColor(color Color) { setElement(max_color, Color); }
		color getMaxColor() { return max_color; }

		void setPercentageColor(color Color) { setElement(percentage_color, Color); }
		color getPercentageColor() { return percentage_color; }
	};

	template <int direction>
	struct bar : surface1D<direction>, bar_appearance
	{
	private:
		float m_min;
		float m_max;
		float m_value;

		bool m_display_value_label = false;
		bool m_display_min_label = false;
		bool m_display_max_label = false;
		bool m_display_percentage_label = false;
		bool m_display_labels_at_end = false;
		int m_labels_precision = -1;

		bool m_redraw_needed = true;

		void fill()
		{
			float distance = fabs(m_min - m_max);

			if (distance > 0)
			{
				console_string val_str;

				if (isDisplayingLabels())
				{
					if (m_display_percentage_label) { val_str += {ToStringP(abs(m_min - m_value) / distance * 100, m_labels_precision) + '%', percentage_color}; }
					if (m_display_min_label) { val_str += {ToStringP(m_min, m_labels_precision) + '/', min_color}; }
					if (m_display_value_label) { val_str += {ToStringP(m_value, m_labels_precision), value_color}; }
					if (m_display_max_label) { val_str += {'/' + ToStringP(m_max, m_labels_precision), max_color}; }
				}

				auto getBarSize = [&]()
				{
					int s = surface1D<direction>::getSize() - val_str.size();
					return s >= 0 ? s : 0;
				};

				int bar_offset = val_str.size() * !m_display_labels_at_end;
				int str_offset = getBarSize() * m_display_labels_at_end;


				int halves = getBarSize() * 2;
				int h = round(fabs(m_min - m_value) / distance * halves);

				int dir_c = (direction == tui::DIRECTION::HORIZONTAL ? 0 : getBarSize() - 1);

				for (int i = 0; i < halves; i += 2)
				{
					if (i < h && i + 1 < h)
					{
						surface1D<direction>::setSymbolAt(full, abs(dir_c - i / 2) + bar_offset);
					}
					else if (i < h && i + 1 >= h)
					{
						surface1D<direction>::setSymbolAt(half, abs(dir_c - i / 2) + bar_offset);
					}
					else
					{
						surface1D<direction>::setSymbolAt(empty, abs(dir_c - i / 2) + bar_offset);
					}
				}

				if (isDisplayingLabels())
				{
					for (int i = 0; i < val_str.size() && i + str_offset < surface1D<direction>::getSize(); i++)
					{
						surface1D<direction>::setSymbolAt(val_str[i], i + str_offset);
					}
				}
			}
			else
			{
				for (int i = 0;i < surface1D<direction>::getSize(); i++)
				{
					surface1D<direction>::setSymbolAt(empty, i);
				}
			}
		}

		template<typename T>
		void setProperty(T& elem_to_set, T elem)
		{
			elem_to_set = elem;
			m_redraw_needed = true;
		}

		void drawAction() override 
		{
			if (m_redraw_needed)
			{
				fill();
				m_redraw_needed = false;
			}
		};
		void resizeAction() override { m_redraw_needed = true; }

		void setAppearanceAction() override { m_redraw_needed = true; }
	public:
		bar() : bar(1) {}
		bar(surface1D_size size) : bar(size, 0,0,0) {}
		bar(surface1D_size size, float min, float max, float value) : m_min(min), m_max(max), m_value(0), bar_appearance(direction)
		{
			setMinValue(min);
			setMaxValue(max);
			setValue(value);

			surface1D<direction>::setSizeInfo(size);
		}

		void setMaxValue(float max)
		{
			if (max < m_min) { max = m_min; }
			setProperty(m_max, max);
		}
		float getMaxValue() { return m_max; }

		void setMinValue(float min)
		{
			if (min > m_max) { min = m_max; }
			setProperty(m_min, min);
		}
		float getMinValue() { return m_min; }

		void setValue(float value)
		{
			if (value < m_min) { value = m_min; }
			if (value > m_max) { value = m_max; }
			setProperty(m_value, value);
		}
		float getValue() { return m_value; }

		void displayLabels(bool display)
		{
			m_display_value_label = display;
			m_display_min_label = display;
			m_display_max_label = display;
			m_display_percentage_label = display;

			m_redraw_needed = true;
		}
		bool isDisplayingLabels() 
		{
			return m_display_value_label || m_display_min_label 
				  || m_display_max_label || m_display_percentage_label;
		}

		void displayValueLabel(bool display) { setProperty(m_display_value_label, display); }
		bool isDisplayingValueLabel() { return m_display_value_label; }

		void displayMinLabel(bool display) { setProperty(m_display_min_label, display); }
		bool isDisplayingMinLabel() { return m_display_min_label; }

		void displayMaxLabel(bool display) { setProperty(m_display_max_label, display); }
		bool isDisplayingMaxLabel() { return m_display_max_label; }

		void displayPercentageLabel(bool display) { setProperty(m_display_percentage_label, display); }
		bool isDisplayingPercentageLabel() { return m_display_percentage_label; }

		void displayLabelsAtEnd(bool display) { setProperty(m_display_labels_at_end, display); }
		bool isDisplayingLabelsAtEnd() { return m_display_labels_at_end; }

		void setLabelsPrecision(int precision) { setProperty(m_labels_precision, precision); }
		int getLabelsPrecision() { return m_labels_precision; }
	};
}
