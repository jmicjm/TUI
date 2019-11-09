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

	};

	template <int direction>
	struct bar : surface1D<direction>, bar_appearance
	{
	private:
		float m_min;
		float m_max;
		float m_value;

		bool m_display_labels = false;
		bool m_display_min_label = true;
		bool m_display_max_label = true;
		bool m_display_labels_at_end = false;
		int m_labels_precison = -1;

		void fill()
		{
			console_string val_str; 
			
			if (m_display_labels)
			{
				if (m_display_min_label) { val_str += ToStringP(m_min, m_labels_precison) + '/'; }
				val_str += ToStringP(m_value, m_labels_precison); 
				if (m_display_max_label) { val_str += '/' + ToStringP(m_max, m_labels_precison); }
			}

			auto getBarSize = [&]()
			{
				int s = surface1D<direction>::getSize() - val_str.size();
				return s >= 0 ? s : 0;
			};

			int bar_offset = val_str.size() * !m_display_labels_at_end;
			int str_offset = getBarSize() * m_display_labels_at_end;


			float distance = fabs(m_min - m_max);
			if (distance > 0)
			{
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
			}

			if (m_display_labels)
			{
				for (int i = 0; i < val_str.size() && i + str_offset < surface1D<direction>::getSize(); i++)
				{
					surface1D<direction>::setSymbolAt(val_str[i], i + str_offset);
				}
			}
		}
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
			m_max = max;
			fill();
		}
		float getMaxValue() { return m_max; }
		void setMinValue(float min)
		{
			if (min > m_max) { min = m_max; }
			m_min = min;
			fill();
		}
		float getMinValue() { return m_min; }
		void setValue(float value)
		{
			if (value < m_min) { value = m_min; }
			if (value > m_max) { value = m_max; }

			m_value = value;
			fill();
		}
		float getValue() { return m_value; }

		void displayLabels(bool display)
		{
			m_display_labels = display;
			fill();
		}
		bool isDisplayingLabels() { return m_display_labels; }

		void displayMinLabel(bool display)
		{
			m_display_min_label = display;
			fill();
		}
		bool isDisplayingMinLabel() { return m_display_min_label; }

		void displayMaxLabel(bool display)
		{
			m_display_max_label = display;
			fill();
		}
		bool isDisplayingMaxLabel() { return m_display_max_label; }

		void displayLabelsAtEnd(bool display)
		{
			m_display_labels_at_end = display;
			fill();
		}
		bool isDisplayingLabelsAtEnd() { return m_display_labels_at_end; }

		void setLabelsPrecision(int precision)
		{
			m_labels_precision = precision;
			fill();
		}
		int getLabelsPrecision() { return m_labels_precision; }

		void drawAction() override {};
		void resizeAction() override { fill(); }

		void setAppearanceAction() override { fill(); }
	};
}
