/*this file contains following elements:
struct box_appearance - describes bar appearance
struct box - widget that displays a box/frame*/
#pragma once
#include "tui_surface.h"
#include "tui_appearance.h"
#include "tui_text_utils.h"

namespace tui
{

	struct box_appearance : appearance
	{
	protected:
		symbol horizontal_top_line;
		symbol horizontal_bottom_line;
		symbol vertical_left_line;
		symbol vertical_right_line;
		symbol top_left;
		symbol top_right;
		symbol bottom_left;
		symbol bottom_right;
	public:
		box_appearance() 
		{
			*this = double_line;
		}
		box_appearance(symbol s) : box_appearance(s, s, s, s, s, s, s, s) {}
		box_appearance(
			symbol ht_line,
			symbol hb_line,
			symbol vl_line,
			symbol vr_line,
			symbol top_l,
			symbol top_r,
			symbol bottom_l,
			symbol bottom_r
		)
			: horizontal_top_line(ht_line),
			horizontal_bottom_line(hb_line),
			vertical_left_line(vl_line),
			vertical_right_line(vr_line),
			top_left(top_l),
			top_right(top_r),
			bottom_left(bottom_l),
			bottom_right(bottom_r) {}

		static const box_appearance thin_line;
		static const box_appearance double_line;
		static const box_appearance medium_line;
		static const box_appearance thick_line;


		void setColor(color Color) override
		{
			horizontal_top_line.setColor(Color);
			horizontal_bottom_line.setColor(Color);
			vertical_left_line.setColor(Color);
			vertical_right_line.setColor(Color);
			top_left.setColor(Color);
			top_right.setColor(Color);
			bottom_left.setColor(Color);
			bottom_right.setColor(Color);
			setAppearanceAction();
		}

		void setAppearance(box_appearance appearance) { setElement(*this, appearance); }
		box_appearance getAppearance() const { return *this; }

		void setHorizontalTopLineSymbol(symbol h_line) { setElement(horizontal_top_line,h_line); }
		symbol getHorizontalTopLineSymbol() const { return horizontal_top_line; }

		void setHorizontalBottomLineSymbol(symbol h_line) { setElement(horizontal_bottom_line, h_line); }
		symbol getHorizontalBottomLineSymbol() const { return horizontal_bottom_line; }

		void setVerticalLeftLineSymbol(symbol v_line) { setElement(vertical_left_line, v_line); }
		symbol getVerticalLeftLineSymbol() const { return vertical_left_line; }

		void setVerticalRightLineSymbol(symbol v_line) { setElement(vertical_right_line, v_line); }
		symbol getVerticalRightLineSymbol() const { return vertical_right_line; }

		void setTopLeftSymbol(symbol top_l) { setElement(top_left, top_l); }
		symbol getTopLeftSymbol() const { return top_left; }

		void setTopRightSymbol(symbol top_r) { setElement(top_right, top_r); }
		symbol getTopRightSymbol() const { return top_right; }

		void setBottomLeftSymbol(symbol bottom_l) { setElement(bottom_left, bottom_l); }
		symbol getBottomLeftSymbol() const { return bottom_left; }

		void setBottomRightSymbol(symbol bottom_r) { setElement(bottom_right, bottom_r); }
		symbol getBottomRightSymbol() const { return bottom_right; }
	};

	struct box : surface, box_appearance
	{
	private:
		symbol_string m_title;
		int m_title_position = POSITION::CENTER;

		bool m_redraw_needed = true;

		void fill()
		{
			clear();

			setSymbolAt(top_left, vec2i(0, 0));
			setSymbolAt(top_right, vec2i(getSize().x - 1, 0));
			setSymbolAt(bottom_left, vec2i(0, getSize().y - 1));
			setSymbolAt(bottom_right, vec2i(getSize().x - 1, getSize().y - 1));

			for (int i = 1; i < getSize().x - 1; i++) { setSymbolAt(horizontal_top_line, vec2i(i, 0)); }
			for (int i = 1; i < getSize().x - 1; i++) { setSymbolAt(horizontal_bottom_line, vec2i(i, getSize().y - 1)); }

			for (int i = 1; i < getSize().y - 1; i++) { setSymbolAt(vertical_left_line, vec2i(0, i)); }
			for (int i = 1; i < getSize().y - 1; i++) { setSymbolAt(vertical_right_line, vec2i(getSize().x - 1, i)); }

			symbol_string fw_str = getFullWidthString(m_title);

			bool shift = getSize().x > fw_str.size();
			int t_pos = (getSize().x-2) * (m_title_position / 100.f) - fw_str.size() * (m_title_position / 100.f) + shift;
			if (t_pos < 0) { t_pos = 0; }

			for (int i = 0; i < fw_str.size() && t_pos < getSize().x; i++, t_pos++)
			{
				setSymbolAt(fw_str[i], vec2i(t_pos, 0));
			}
		}
		void drawAction(action_proxy proxy) override
		{
			if (m_redraw_needed) 
			{
				fill(); 
				m_redraw_needed = false;
			}
		}
		void resizeAction() override { m_redraw_needed = true; }
		void setAppearanceAction() override { m_redraw_needed = true; }
	public:
		box(surface_size size = surface_size())
		{
			setSizeInfo(size);
		}

		void setTitle(const symbol_string& str)
		{
			m_title = str;
			m_redraw_needed = true;
		}
		symbol_string getTitle() const { return m_title; }

		void setTitlePosition(int position)
		{
			m_title_position = position;
			m_redraw_needed = true;
		}
		int getTitlePosition() const { return m_title_position; }
	};
}