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
		std::vector<float> m_values;
		tui::scroll<tui::DIRECTION::HORIZONTAL> m_scroll;
		int m_distance = 3;

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

			m_scroll.setContentLength(m_values.size() * m_distance);

			if (tui::KEYBOARD::isKeyPressed(key_right))
			{
				m_scroll.setHandlePosition(m_scroll.getHandlePosition() + 1);
			}
			if (tui::KEYBOARD::isKeyPressed(key_left))
			{
				m_scroll.setHandlePosition(m_scroll.getHandlePosition() - 1);
			}

			auto getHeight = [&]()
			{
				if (m_scroll.isNeeded()) { return getSize().y - 1; }
				else { return getSize().y; }
			};

			int distance = ceil(fabs(min - max));
			if (distance > 0)
			{
				if (min > 0)
				{
					distance += min;
				}
				int zero_offset = 0;
				if (min < 0)
				{
					zero_offset = (min / distance) * getHeight();
				}

				int h_pos = m_scroll.getHandlePosition();
				int x = m_distance *(h_pos % m_distance != 0) - h_pos % m_distance;

				for (int i = ceil(h_pos / (float)m_distance); (i < m_values.size() && x < getSize().x); i++, x += m_distance)
				{
					for (int j = 0; j < round(fabs(m_values[i]) / distance * getHeight()); j++)
					{
						surface::setSymbolAt(full, { x, m_values[i] < 0 ? getHeight() + zero_offset + j : getHeight() - 1 - j + zero_offset });
					}
				}
			}
			insertSurface(m_scroll);
		}

	public:
		int key_left = tui::KEYBOARD::KEY::LEFT;
		int key_right = tui::KEYBOARD::KEY::RIGHT;
		chart() : m_scroll({0,100}) 
		{
			m_scroll.setPosition({ { 0,-1 }, { 0,100 } });
			m_scroll.activate();
		}

		void setValues(std::vector<float> values) 
		{ 
			m_values = values;
			fill();
		}
		std::vector<float> getValues() { return m_values; }


		void draw_action() override { fill(); }

	};


}