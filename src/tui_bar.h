#pragma once
#include "tui_console.h"
#include "tui_appearance.h"
#include "tui_text_utils.h"

namespace tui
{
	struct bar_appearance : appearance
	{
	protected:
		symbol full;
		symbol empty;
	public:
		bar_appearance() : bar_appearance({ U'\x2588', COLOR::WHITE },{ U'\x2588', COLOR::DARKGRAY }) {}
		bar_appearance(symbol Full, symbol Empty) : full(Full), empty(Empty) {}

		void setColor(color Color) override
		{
			full.setColor(Color);
			empty.setColor(Color);
			setAppearance_action();
		}

		void setAppearance(bar_appearance appearance)
		{
			*this = appearance;
			setAppearance_action();
		}
		bar_appearance getAppearance() { return *this; }

		void setFullSymbol(symbol Full)
		{
			full = Full;
			setAppearance_action();
		}
		symbol getFullSymbol() { return full; }
		void setEmptySymbol(symbol Empty)
		{
			empty = Empty;
			setAppearance_action();
		}
		symbol getEmptySymbol() { return empty; }
	};

	template <int direction>
	struct bar : surface1D<direction>, bar_appearance
	{
	private:
		float m_min;
		float m_max;
		float m_value;

		void fill()
		{
			float bar_len = abs(m_min - m_max);
			float value_len = abs(m_min - m_value);

			float perc = value_len / bar_len;
			int f_l = round(perc * surface1D<direction>::getSize());

			surface::makeTransparent();

			for (int i = 0; i < surface1D<direction>::getSize(); i++)
			{
				if (i < f_l) { surface1D<direction>::setSymbolAt(full, i); }
				else { surface1D<direction>::setSymbolAt(empty, i); }
			}
		}
	public:
		bar(surface1D_size size, float min, float max, float value) : m_min(0), m_max(0), m_value(0)
		{
			setMinValue(min);
			setMaxValue(max);
			setValue(value);

			surface1D<direction>::setSize(size);
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

		void draw_action() override {};
		void resize_action() override { fill(); }

		void setAppearance_action() override { fill(); }
	};
}
