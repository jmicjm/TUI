#pragma once
#include "tui_surface.h"
#include "tui_appearance.h"
#include "tui_text_utils.h"
#include "tui_active_element.h"
#include "tui_scroll.h"

#include <vector>
#include <algorithm>

namespace tui
{
	struct chart_appearance : appearance
	{
	protected:
		symbol full;
		symbol lower_half;
		symbol upper_half;
		scroll_appearance chart_scroll_appearance;
		color value_labels_color;
	public:
		chart_appearance() : chart_appearance(U'\x2588', U'\x2584', U'\x2580') {}
		chart_appearance(symbol Full, symbol Lower, symbol Upper) 
			: full(Full), lower_half(Lower), upper_half(Upper), chart_scroll_appearance(tui::DIRECTION::HORIZONTAL) {}

		void setColor(color Color) override
		{
			full.setColor(Color);
			lower_half.setColor(Color);
			upper_half.setColor(Color);
			value_labels_color = Color;
			chart_scroll_appearance.setColor(Color);
			setAppearanceAction();
		}
		void setAppearance(chart_appearance appearance) { setElement(*this, appearance); }
		chart_appearance getAppearance() { return *this; }

		void setFullSymbol(symbol Full) { setElement(full, Full); }
		symbol getFullSymbol() { return full; }

		void setLowerHalfSymbol(symbol Lower) { setElement(lower_half, Lower); }
		symbol getLowerHalfSymbol() { return lower_half; }

		void setUpperHalfSymbol(symbol Upper) { setElement(upper_half, Upper); }
		symbol getUpperHalfSymbol() { return upper_half; }

		void setScrollAppearance(scroll_appearance scroll) { setElement(chart_scroll_appearance, scroll); }
		scroll_appearance getScrollAppearance() { return chart_scroll_appearance; }

		void setValueLabelsColor(color Color) { setElement(value_labels_color, Color); }
		color getValueLabelsColor() { return value_labels_color; }
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

		void updateMinMaxStr()
		{
			m_max_str = m_max > 0 ? console_string(ToStringP(m_max, m_value_labels_precision), value_labels_color) : "0";
			m_min_str = m_min < 0 ? console_string(ToStringP(m_min, m_value_labels_precision), value_labels_color) : "0";
		}

		void fill()
		{
			makeTransparent();

			unsigned short label_str_width = m_display_value_labels * (m_max_str.size() > m_min_str.size() ? m_max_str.size() : m_min_str.size());

			m_scroll.setContentLength(m_values.size() * m_distance - (m_distance-1));
			m_scroll.setVisibleContentLength(getSize().x - label_str_width);
			insertSurface(m_scroll, false);

			if (m_scroll.isNeeded()) { m_chart.setSizeInfo({ {label_str_width * -1,-1},{100,100} }); }
			else { m_chart.setSizeInfo({ {label_str_width * -1,0},{100,100} }); }
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

				int scroll_pos = m_scroll.getTopPosition();
				int x = m_distance *(scroll_pos % m_distance != 0) - scroll_pos % m_distance;

				for (int i = ceil(scroll_pos / (float)m_distance); (i < m_values.size() && x < m_chart.getSize().x); i++, x += m_distance)
				{
					int h = round(fabs(m_values[i]) / distance * halves);

					auto isFull = [&](int y)
					{
						switch (m_values[i] >= 0)
						{
						case true:
							return y >= p_halves - h && y < p_halves;
						case false:
							return y >= p_halves && y < p_halves + h;
						}
					};
						
					for (int y = 0; y < halves; y+=2)
					{
						if (isFull(y) && isFull(y + 1))
						{
							m_chart.setSymbolAt(full, { x, y / 2 });
						}
						else if (isFull(y) && !isFull(y + 1))
						{
							m_chart.setSymbolAt(upper_half, { x, y / 2 });
						}
						else if (!isFull(y) && isFull(y + 1))
						{
							m_chart.setSymbolAt(lower_half, { x, y / 2 });
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
			updateMinMaxStr();
			m_redraw_needed = true; 
		}

	public:
		chart() : m_scroll({0,100}) 
		{
			m_scroll.setPositionInfo({ { 0,-1 }, { 0,100 } });
			m_chart.setPositionInfo({ {0,0}, {0,0}, {tui::POSITION::END, tui::POSITION::BEGIN} });
		}

		void setValues(std::vector<float> values)
		{
			m_values = values;

			m_min = 0;
			m_max = 0;
			if (m_values.size() > 0)
			{
				m_min = *std::min_element(m_values.begin(), m_values.end());
				m_max = *std::max_element(m_values.begin(), m_values.end());
			}

			updateMinMaxStr();

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
			int old_scroll_handle_pos = m_scroll.getTopPosition();
			m_scroll.update();
			if (m_scroll.getTopPosition() != old_scroll_handle_pos)
			{
				m_redraw_needed = true;
			}
		}
	};
}