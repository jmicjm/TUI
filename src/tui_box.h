#pragma once
#include "tui_surface.h"
#include "tui_appearance.h"
#include "tui_text_utils.h"
#include "tui_utils.h"

namespace tui
{

	struct box_appearance : appearance
	{
	protected:
		symbol m_horizontal_line;
		symbol m_vertical_line;
		symbol m_top_left;
		symbol m_top_right;
		symbol m_bottom_left;
		symbol m_bottom_right;
	public:
		box_appearance() : box_appearance(U'\x2550', U'\x2551', U'\x2554', U'\x2557', U'\x255A', U'\x255D') {}
		box_appearance(symbol Symbol) : box_appearance(Symbol, Symbol, Symbol, Symbol, Symbol, Symbol) {}
		box_appearance(symbol h_line, symbol v_line, symbol top_l, symbol top_r, symbol bottom_l, symbol bottom_r)
			: m_horizontal_line(h_line), m_vertical_line(v_line), m_top_left(top_l),
			m_top_right(top_r), m_bottom_left(bottom_l), m_bottom_right(bottom_r) {}

		void setColor(color Color) override
		{
			m_horizontal_line.setColor(Color);
			m_vertical_line.setColor(Color);
			m_top_left.setColor(Color);
			m_top_right.setColor(Color);
			m_bottom_left.setColor(Color);
			m_bottom_right.setColor(Color);
			setAppearanceAction();
		}

		void setAppearance(box_appearance appearance) { setElement(*this, appearance); }
		box_appearance getAppearance() { return *this; }

		void setHorizontalLine(symbol h_line) { setElement(m_horizontal_line,h_line); }
		symbol getHorizontalLine() { return m_horizontal_line; }

		void setVerticalLine(symbol v_line) { setElement(m_vertical_line, v_line); }
		symbol getVerticalLine() { return m_vertical_line; }

		void setTopLeft(symbol top_l) { setElement(m_top_left, top_l); }
		symbol getTopLeft() { return m_top_left; }

		void setTopRight(symbol top_r) { setElement(m_top_right, top_r); }
		symbol getTopRight() { return m_top_right; }

		void setBottomLeft(symbol bottom_l) { setElement(m_bottom_left, bottom_l); }
		symbol getBottomLeft() { return m_bottom_left; }

		void setBottomRight(symbol bottom_r) { setElement(m_bottom_right, bottom_r); }
		symbol getBottomRight() { return m_bottom_right; }

	};

	struct box : surface, box_appearance
	{
	private:
		symbol_string m_title;
		int m_title_position = POSITION::CENTER;

		bool m_redraw_needed = true;

		void fill()
		{
			setSymbolAt(m_top_left, vec2i(0, 0));
			setSymbolAt(m_top_right, vec2i(getSize().x - 1, 0));
			setSymbolAt(m_bottom_left, vec2i(0, getSize().y - 1));
			setSymbolAt(m_bottom_right, vec2i(getSize().x - 1, getSize().y - 1));

			for (int i = 1; i < getSize().x - 1; i++) { setSymbolAt(m_horizontal_line, vec2i(i, 0)); }
			for (int i = 1; i < getSize().x - 1; i++) { setSymbolAt(m_horizontal_line, vec2i(i, getSize().y - 1)); }

			for (int i = 1; i < getSize().y - 1; i++) { setSymbolAt(m_vertical_line, vec2i(0, i)); }
			for (int i = 1; i < getSize().y - 1; i++) { setSymbolAt(m_vertical_line, vec2i(getSize().x - 1, i)); }

			int shift = getSize().x > m_title.size();
			int t_pos = (getSize().x-2) * (m_title_position / 100.f) - m_title.size() * (m_title_position / 100.f) + shift;
			if (t_pos < 0) { t_pos = 0; }

			for (int i = 0; i<m_title.size() && t_pos < getSize().x; i++, t_pos++)
			{
				setSymbolAt(m_title[i], vec2i(t_pos, 0));
			}
		}
		void drawAction() override
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
		box() : box({ {3,3} }) {}
		box(surface_size size) : box(size, box_appearance()) {}
		box(surface_size size, box_appearance appearance)
		{
			setSizeInfo(size);
			setAppearance(appearance);
		}

		void setTitle(const symbol_string& str)
		{
			m_title = str;
			m_redraw_needed = true;
		}
		symbol_string getTitle() { return m_title; }

		void setTitlePosition(int position)
		{
			m_title_position = position;
			m_redraw_needed = true;
		}
		int getTitlePosition() { return m_title_position; }
	};
}