#pragma once
#include "tui_surface.h"
#include "tui_appearance.h"
#include "tui_text_utils.h"

namespace tui
{
	struct rectangle_appearance : appearance
	{
	protected:
		symbol filling;
	public:
		rectangle_appearance() : rectangle_appearance(U'\x2588') {}
		rectangle_appearance(symbol Filling) : filling(Filling) {}

		void setColor(color Color) override
		{
			filling.setColor(Color);
			setAppearanceAction();
		}

		void setAppearance(rectangle_appearance appearance) { setElement(*this, appearance); }
		rectangle_appearance getAppearance() { return *this; }

		void setFilling(symbol Filling) { setElement(filling, Filling); }
		symbol getFilling() { return filling; }
	};

	struct rectangle : surface, rectangle_appearance
	{
	private:
		bool m_redraw_needed = true;
		void fill()
		{
			surface::fill(filling);
		}
		void drawAction() override
		{
			if (m_redraw_needed) { fill(); }
			m_redraw_needed = false;
		}
		void resizeAction() override { m_redraw_needed = true; }
		void setAppearanceAction() override { m_redraw_needed = true; }
	public:
		rectangle() : rectangle({{ 1,1 }}) {}
		rectangle(surface_size size) : rectangle(size, rectangle_appearance()) {}
		rectangle(surface_size size, rectangle_appearance appearance)
		{
			setSizeInfo(size);
			setAppearance(appearance);
		}
	};
}
