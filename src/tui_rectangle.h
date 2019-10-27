#pragma once
#include "tui_surface.h"
#include "tui_appearance.h"
#include "tui_text_utils.h"

namespace tui
{
	struct rectangle_appearance : appearance
	{
	protected:
		symbol m_filling;
	public:
		rectangle_appearance() : rectangle_appearance(U'\x2588') {}
		rectangle_appearance(symbol filling) : m_filling(filling) {}

		void setColor(color Color) override
		{
			m_filling.setColor(Color);
			setAppearanceAction();
		}

		void setAppearance(rectangle_appearance appearance)
		{
			*this = appearance;
			setAppearanceAction();
		}
		rectangle_appearance getAppearance() { return *this; }

		void setFilling(symbol filling)
		{
			m_filling = filling;
			setAppearanceAction();
		}
		symbol getFilling() { return m_filling; }
	};

	struct rectangle : surface, rectangle_appearance
	{
	private:
		void fill()
		{
			for (int y = 0; y < getSize().y; y++)
			{
				for (int x = 0; x < getSize().x; x++)
				{
					setSymbolAt(m_filling, vec2i(x,y));
				}
			}
		}
		void resizeAction() override { fill(); }
		void setAppearanceAction() override { fill(); }
	public:
		rectangle() : rectangle({{ 1,1 }}) {}
		rectangle(surface_size size) : rectangle(size, rectangle_appearance()) {}
		rectangle(surface_size size, rectangle_appearance appearance)
		{
			setSize(size);
			setAppearance(appearance);
		}
	};
}
