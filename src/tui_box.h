#pragma once
#include "tui_console.h"
#include "tui_appearance.h"
#include "tui_text_utils.h"

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

		void setAppearance(box_appearance appearance)
		{
			*this = appearance;
			setAppearanceAction();
		}
		box_appearance getAppearance() { return *this; }

		void setHorizontalLine(symbol line)
		{
			m_horizontal_line = line;
			setAppearanceAction();
		}
		symbol getHorizontalLine() { return m_horizontal_line; }
		void setVerticalLine(symbol line)
		{
			m_vertical_line = line;
			setAppearanceAction();
		}
		symbol getVerticalLine() { return m_vertical_line; }
		void setTopLeft(symbol top_l)
		{
			m_top_left = top_l;
			setAppearanceAction();
		}
		symbol getTopLeft() { return m_top_left; }
		void setTopRight(symbol top_r)
		{
			m_top_right = top_r;
			setAppearanceAction();
		}
		symbol getTopRight() { return m_top_right; }
		void setBottomLeft(symbol bottom_l)
		{
			m_bottom_left = bottom_l;
			setAppearanceAction();
		}
		symbol getBottomLeft() { return m_bottom_left; }
		void setBottomRight(symbol bottom_r)
		{
			m_bottom_right = bottom_r;
			setAppearanceAction();
		}
		symbol getBottomRight() { return m_bottom_right; }

	};

	struct box : surface, box_appearance
	{
	private:
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
		}
		void resizeAction() override { fill(); }
		void setAppearanceAction() override { fill(); }
	public:
		box() : box({ {3,3} }) {}
		box(surface_size size) : box(size, box_appearance()) {}
		box(surface_size size, box_appearance appearance)
		{
			setSize(size);
			setAppearance(appearance);
		}
	};
}