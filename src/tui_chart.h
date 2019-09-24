#pragma once
#include "tui_console.h"
#include "tui_appearance.h"
#include "tui_text_utils.h"
#include "tui_scroll.h"
#include "tui_bar.h"
#include <vector>

namespace tui
{
	struct chart_appearance : appearance
	{
	protected:
		symbol full;
	public:
		chart_appearance() : chart_appearance({ U'\x2588', COLOR::WHITE }) {}
		chart_appearance(symbol Full) : full(Full) {}

		void setColor(color Color) override
		{
			full.setColor(Color);
			setAppearance_action();
		}
	};



	struct chart : surface, chart_appearance
	{
	private:
		std::vector<float> m_values = { 5, 3, -2, 5, 6, -6 };
		tui::scroll<tui::DIRECTION::HORIZONTAL> m_scroll;

		void fill()
		{
			surface::makeTransparent();

			auto getMin = [&]()
			{
				if (m_values.size() == 0) { return (float)0; }
				else
				{
					float min = m_values[0];
					{
						for (int i = 0; i < m_values.size(); i++)
						{
							if (m_values[i] < min) { min = m_values[i]; }
						}
					}
					return min;
				}
			};
			auto getMax = [&]()
			{
				if (m_values.size() == 0) { return (float)0; }
				else
				{
					float max = m_values[0];
					{
						for (int i = 0; i < m_values.size(); i++)
						{
							if (m_values[i] > max) { max = m_values[i]; }
						}
					}
					return max;
				}
			};
			float min = getMin();
			float max = getMax();

			int distance = ceil(abs(min - max));
			if (distance > 0)
			{
				if (min > 0)
				{
					distance += min;
				}
				int zero_offset = 0;
				if (min < 0)
				{
					zero_offset = (min / distance) * getSize().y;
				}

				for (int i = 0; i < m_values.size() && i < getSize().x; i++)
				{
					
						for (int j = 0; j < floor(abs(m_values[i]) / distance * getSize().y); j++)
						{
							surface::setSymbolAt(full, { i * 2, m_values[i] < 0 ? getSize().y + zero_offset + j : getSize().y - 1 - j + zero_offset });
						}
					
	
				}
			}
		}

	public:
		chart() : m_scroll(1) {}




		void draw_action() override { fill(); }

	};


}