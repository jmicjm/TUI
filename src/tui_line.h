#pragma once
#include "tui_surface.h"
#include "tui_appearance.h"
#include "tui_text_utils.h"

namespace tui
{
	struct line_appearance : appearance
	{
	protected:
		symbol filling;
		symbol begin;
		symbol end;
	public:
		line_appearance(DIRECTION direction = DIRECTION::HORIZONTAL)
		{
			switch (direction)
			{
			case DIRECTION::HORIZONTAL:
				filling = U'\x2550';
				begin = U'\x2550';
				end = U'\x2550';
				break;
			case DIRECTION::VERTICAL:
				filling = U'\x2551';
				begin = U'\x2551';
				end = U'\x2551';
			}
		}
		line_appearance(symbol Symbol) : line_appearance(Symbol, Symbol, Symbol) {}
		line_appearance(symbol Filling, symbol Begin, symbol End) : filling(Filling), begin(Begin), end(End) {}

		void setColor(color Color) override
		{
			filling.setColor(Color);
			begin.setColor(Color);
			end.setColor(Color);

			setAppearanceAction();
		}

		void setAppearance(line_appearance appearance) { setElement(*this, appearance); }
		line_appearance getAppearance() { return *this; }

		void setFillingSymbol(symbol Filling) { setElement(filling, Filling); }
		symbol getFillingSymbol() { return filling; }

		void setBeginSymbol(symbol Begin) { setElement(begin, Begin); }
		symbol getBeginSymbol() { return begin; }

		void setEndSymbol(symbol End) { setElement(end, End); }
		symbol getEndSymbol() { return end; }

	};

	template <DIRECTION direction>
	struct line : surface1D<direction>, line_appearance
	{
	private:
		bool m_redraw_needed = true;

		void fill()
		{
			surface1D<direction>::setSymbolAt(begin, 0);
			surface1D<direction>::setSymbolAt(end, surface1D<direction>::getSize() - 1);

			for (int i = 1; i < surface1D<direction>::getSize() - 1; i++)
			{
				surface1D<direction>::setSymbolAt(filling, i);
			}
		}

		void resizeAction() override { m_redraw_needed = true; }
		void setAppearanceAction() override { m_redraw_needed = true; }
		void drawAction() override
		{
			if (m_redraw_needed) { fill(); }
			m_redraw_needed = false;
		}
	public:
		line() : line_appearance(direction) {}
	};
}