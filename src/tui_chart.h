#pragma once
#include "tui_console.h"
#include "tui_appearance.h"
#include "tui_text_utils.h"
#include "tui_scroll.h"
#include <vector>
#include <sstream>
#include <iomanip>

namespace tui
{
	struct chart_appearance : appearance
	{
	protected:
		symbol full;
		symbol lower_half;
		symbol upper_half;
		scroll_appearance chart_scroll_appearance;
	public:
		chart_appearance() : chart_appearance(U'\x2588', U'\x2584', U'\x2580') {}
		chart_appearance(symbol Full, symbol Lower, symbol Upper) : full(Full), lower_half(Lower), upper_half(Upper) {}

		void setColor(color Color) override
		{
			full.setColor(Color);
			setAppearanceAction();
		}
		void setAppearance(chart_appearance appearance) 
		{
			*this = appearance; 
			setAppearanceAction();
		}
		chart_appearance getAppearance() { return *this; }
		void setFullSymbol(symbol Full) 
		{
			full = Full;
			setAppearanceAction();
		}
		symbol getFullSymbol() { return full; }
		void setLowerHalfSymbol(symbol Lower)
		{
			lower_half = Lower;
			setAppearanceAction();
		}
		symbol getLowerHalfSymbol() { return lower_half; }
		void setUpperHalfSymbol(symbol Upper)
		{
			upper_half = Upper;
			setAppearanceAction();
		}
		symbol getUpperHalfSymbol() { return upper_half; }

		void setScrollAppearance(scroll_appearance scroll)
		{
			chart_scroll_appearance = scroll;
			setAppearanceAction();
		}
		scroll_appearance getScrollAppearance() { return chart_scroll_appearance; }
	};

	struct chart : surface, chart_appearance, active_element
	{
	private:
		std::vector<float> m_values;
		tui::scroll<tui::DIRECTION::HORIZONTAL> m_scroll;
		tui::surface m_chart;
		unsigned int m_distance = 2;
		bool m_display_value_labels = false;
		int m_value_labels_precision = -1;

		float m_min = 0;
		float m_max = 0;
		console_string m_min_str;
		console_string m_max_str;

		bool m_redraw_needed = true;

		void fill()
		{
			makeTransparent();

			unsigned short range_width = m_display_value_labels * (m_max_str.size() > m_min_str.size() ? m_max_str.size() : m_min_str.size());

			m_scroll.setContentLength(m_values.size() * m_distance - (m_distance-1));
			m_scroll.setVisibleContentLength(getSize().x - range_width);
			insertSurface(m_scroll, false);

			if (m_scroll.isNeeded()) { m_chart.setSize({ {range_width * -1,-1},{100,100} }); }
			else { m_chart.setSize({ {range_width * -1,0},{100,100} }); }
			updateSurfaceSize(m_chart);

			if (m_display_value_labels)
			{
				for (int i = 0; i < m_max_str.size(); i++)
				{
					setSymbolAt(m_max_str[i], { i,0 });
				}
				for (int i = 0; i < m_min_str.size(); i++)
				{
					setSymbolAt(m_min_str[i], { i,m_chart.getSize().y - 1 });
				}
			}

			float distance = (fabs(m_min - m_max));
			if (m_min > 0) { distance += m_min; }
			if (m_max < 0) { distance += fabs(m_max); }

			if (distance > 0)
			{
				int halves = m_chart.getSize().y * 2;
				int p_halves = round(m_max / distance * halves) * (m_max>=0);

				int h_pos = m_scroll.getHandlePosition();
				int x = m_distance *(h_pos % m_distance != 0) - h_pos % m_distance;

				for (int i = ceil(h_pos / (float)m_distance); (i < m_values.size() && x < m_chart.getSize().x); i++, x += m_distance)
				{
					int h = round(fabs(m_values[i]) / distance * halves);

					auto isFull = [&](int y)
					{
						switch (m_values[i] >= 0)
						{
						case true:
							if (y >= p_halves - h && y < p_halves) { return true; }
							break;
						case false:
							if (y >= p_halves && y < p_halves + h) { return true; }
						}
						return false;
					};
						
					for (int j = 0; j < halves; j+=2)
					{
						if (isFull(j) && isFull(j + 1))
						{
							m_chart.setSymbolAt(full, { x, j / 2 });
						}
						else if (isFull(j) && !isFull(j + 1))
						{
							m_chart.setSymbolAt(upper_half, { x, j / 2 });
						}
						else if (!isFull(j) && isFull(j + 1))
						{
							m_chart.setSymbolAt(lower_half, { x, j / 2 });
						}
					}	
				}
			}
			insertSurface(m_chart);
		}

		void resizeAction() override { m_redraw_needed = true; }
		void updateAction() override { update(); }
		void drawAction() override
		{
			if (m_redraw_needed)
			{
				fill();
				m_redraw_needed = false;
			}
		}

		void activationAction() override
		{
			m_scroll.activate();
			if (m_scroll.isNeeded()) { m_redraw_needed = true; }
		}
		void disactivationAction() override
		{
			m_scroll.disactivate();
			if (m_scroll.isNeeded()) { m_redraw_needed = true; }
		}

		void setAppearanceAction() override 
		{
			m_scroll.setAppearance(chart_scroll_appearance);
			m_redraw_needed = true; 
		}

	public:
		chart() : m_scroll({0,100}) 
		{
			m_scroll.setPosition({ { 0,-1 }, { 0,100 } });
			m_chart.setPosition({ {0,0}, {0,0}, {tui::POSITION::HORIZONTAL::RIGHT, tui::POSITION::VERTICAL::TOP} });
		}

		void setValues(std::vector<float> values) 
		{ 
			auto getMin = [&]()
			{
				if (values.size() == 0) { return (float)0; }
				else
				{
					float min = values[0];
					{
						for (int i = 0; i < values.size(); i++)
						{
							if (values[i] < min) { min = values[i]; }
						}
					}
					return min;
				}
			};
			auto getMax = [&]()
			{
				if (values.size() == 0) { return (float)0; }
				else
				{
					float max = values[0];
					{
						for (int i = 0; i < values.size(); i++)
						{
							if (values[i] > max) { max = values[i]; }
						}
					}
					return max;
				}
			};
			m_min = getMin();
			m_max = getMax();

			auto to_string_p = [](float val, int precision)
			{
				std::stringstream ss_val;
				if (precision >= 0)
				{
					ss_val << std::fixed << std::setprecision(precision);
				}
				ss_val << val;
				std::string s_val = ss_val.str();

				for (int i = s_val.size() - 1; i > 0; i--)
				{
					if (s_val[i] == '0') { s_val.pop_back(); }
					else if (s_val[i] == '.')
					{
						s_val.pop_back();
						break;
					}
					else { break; }
				}

				return s_val;
			};

			m_max_str = to_string_p(m_max, m_value_labels_precision);
			m_min_str = to_string_p(m_min, m_value_labels_precision);

			m_values = values;
			m_redraw_needed = true;
		}
		std::vector<float> getValues() { return m_values; }

		void setDistance(unsigned int distance)
		{
			if (distance > 0) { m_distance = distance; }
			else { m_distance = 1; }
			m_redraw_needed = true;
		}
		unsigned int getDistance() { return m_distance; }

		void displayValueLabels(bool display)
		{
			m_display_value_labels = display;
			m_redraw_needed = true;
		}
		bool isDisplayingValueLabels() { return m_display_value_labels; }

		void setValueLabelsPrecision(int precision)
		{
			m_value_labels_precision = precision;
			m_redraw_needed = true;
		}
		int getValueLabelsPrecision() { return m_value_labels_precision; }

		void update()
		{
			int old_scroll_handle_pos = m_scroll.getHandlePosition();
			m_scroll.update();
			if (m_scroll.getHandlePosition() != old_scroll_handle_pos)
			{
				m_redraw_needed = true;
			}
		}
	};
}