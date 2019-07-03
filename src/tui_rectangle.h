#pragma once
#include "tui_console.h"
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
			setAppearance_action();
		}

		void setAppearance(rectangle_appearance appearance)
		{
			*this = appearance;
			setAppearance_action();
		}
		rectangle_appearance getAppearance() { return *this; }

		void setFilling(symbol filling)
		{
			m_filling = filling;
			setAppearance_action();
		}
		symbol getFilling() { return m_filling; }
	};

	struct rectangle : surface, rectangle_appearance
	{
	private:
		void fill()
		{
			for (int i = 0; i < getSize().x; i++)
			{
				for (int j = 0; j < getSize().y; j++)
				{
					setSymbolAt(m_filling, vec2i(i, j));
				}
			}
		}
		void resize_action() override { fill(); }
		void setAppearance_action() override { fill(); }
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
