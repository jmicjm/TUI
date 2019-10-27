#pragma once
#include <vector>
#include "tui_utils.h"
#include "tui_text_utils.h"

namespace tui
{
	struct surface
	{
	private:
		std::vector<symbol> m_symbols;
		unsigned int m_width = 0;
		position m_position;
		surface_size size_info;

		bool m_resized;

		void resize(vec2i size)
		{
			if (size.x != getSize().x || size.y != getSize().y)
			{
				vec2i new_size = size;
				if (size.x < 1) { new_size.x = 1; }
				if (size.y < 1) { new_size.y = 1; }

				m_symbols.resize(new_size.x * new_size.y);
				m_width = new_size.x;

				makeTransparent();
				resizeAction();

				m_resized = true;
			}
			else
			{
				m_resized = false;
			}
		}

	protected:
		virtual void resizeAction() {}
		virtual void updateAction() {}
		virtual void drawAction() {}
	public:
		surface() : surface({ {1,1},{0,0} }) {}
		surface(surface_size size)
		{
			setSize(size);
		}

		bool isResized()
		{
			return m_resized;
		}

		struct surface_proxy
		{
			friend class surface;
		private:
			surface* surf;
			int x;
		public:
			symbol& operator[](int y)
			{
				return surf->m_symbols[surf->m_width * y + x];
			}
		};

		surface_proxy operator[](int x)
		{
			surface_proxy proxy;
			proxy.surf = this;
			proxy.x = x;

			return proxy;
		}

		void setSymbolAt(symbol character, vec2i position) { m_symbols[position.y * m_width + position.x] = character; }
		symbol getSymbolAt(vec2i position) { return m_symbols[position.y * m_width + position.x]; }
		void setPosition(position pos) { m_position = pos; }

		void move(vec2i offset)
		{
			vec2i act_pos = getPosition().getOffset();
			m_position.setOffset(vec2i(act_pos.x + offset.x, act_pos.y + offset.y));
		}

		void setSize(surface_size size)
		{
			size_info = size;
			resize(size.getFixedSize());
		}

		void updateSurfaceSize(surface& obj)
		{
			vec2i perc_size = obj.getSizeInfo().getPercentageSize();
			vec2i int_size = obj.getSizeInfo().getFixedSize();

			int x = (perc_size.x / 100.f) * getSize().x + int_size.x;
			int y = (perc_size.y / 100.f) * getSize().y + int_size.y;

			obj.resize({ x,y });
		}

		void insertSurface(surface& obj, bool update = true)
		{
			updateSurfaceSize(obj);

			if (update) { obj.updateAction(); }

			obj.drawAction();


			int x_origin = getSize().x * (obj.getPosition().getRelativePoint().x / 100.f) - obj.getSize().x * (obj.getPosition().getRelativePoint().x / 100.f);
			x_origin += obj.getPosition().getOffset().x;
			x_origin += obj.getPosition().getPercentageOffset().x * getSize().x / 100.f;

			int y_origin = getSize().y * (obj.getPosition().getRelativePoint().y / 100.f) - obj.getSize().y * (obj.getPosition().getRelativePoint().y / 100.f);
			y_origin += obj.getPosition().getOffset().y;
			y_origin += obj.getPosition().getPercentageOffset().y * getSize().y / 100.f;

			for (int y = 0; y < obj.getSize().y; y++)
			{
				for (int x = 0; x < obj.getSize().x; x++)
				{
					if (x_origin + x < getSize().x
						&& y_origin + y < getSize().y
						&& x_origin + x >= 0
						&& y_origin + y >= 0
						&& obj[x][y].getFirstChar() != 0)
					{
						setSymbolAt(obj.getSymbolAt({ x, y }), { x_origin + x, y_origin + y });
					}
				}
			}
		}

		void fill(symbol Symbol)
		{
			for (int i = 0; i < m_symbols.size(); i++) { m_symbols[i] = Symbol; }
		}

		void invert()
		{
			for (int i = 0; i < m_symbols.size(); i++) { m_symbols[i].invert(); }
		}

		void makeTransparent() { fill((char32_t)0); }
		void makeBlank() { fill(BLANKSYMBOL); }

		position getPosition() { return m_position; }
		vec2i getSize()
		{
			if (m_width > 0) { return vec2i(m_width, m_symbols.size() / m_width); }
			else { return vec2i(0, 0); }
		}
		surface_size getSizeInfo() { return size_info; }

	};

	template<int direction>
	struct surface1D : surface
	{
		void setSize(surface1D_size size)
		{
			switch (direction)
			{
			case tui::DIRECTION::HORIZONTAL:
				surface::setSize({ {size.fixed_size, 1},{size.percentage_size, 0} });
				break;
			case tui::DIRECTION::VERTICAL:
				surface::setSize({ {1, size.fixed_size},{0, size.percentage_size} });
				break;
			}
		}
		int getSize()
		{
			switch (direction)
			{
			case tui::DIRECTION::HORIZONTAL:
				return surface::getSize().x;
			case tui::DIRECTION::VERTICAL:
				return surface::getSize().y;
			}
		}
		void setSymbolAt(symbol character, int position)
		{
			switch (direction)
			{
			case tui::DIRECTION::HORIZONTAL:
				surface::setSymbolAt(character, { position, 0 });
				break;
			case tui::DIRECTION::VERTICAL:
				surface::setSymbolAt(character, { 0, position });
				break;
			}
		}
		symbol getSymbolAt(int position)
		{
			switch (direction)
			{
			case tui::DIRECTION::HORIZONTAL:
				return surface::getSymbolAt({ position, 0 });
			case tui::DIRECTION::VERTICAL:
				return surface::getSymbolAt({ 0, position });
			}
		}
		symbol& operator[](int i)
		{
			switch (direction)
			{
			case tui::DIRECTION::HORIZONTAL:
				return surface::operator[](i)[0];
			case tui::DIRECTION::VERTICAL:
				return surface::operator[](0)[i];
			}
		}
		surface1D_size getSizeInfo()
		{
			switch (direction)
			{
			case tui::DIRECTION::HORIZONTAL:
				return surface1D_size(surface::getSizeInfo().getFixedSize().x, surface::getSizeInfo().getPercentageSize().x);
			case tui::DIRECTION::VERTICAL:
				return surface1D_size(surface::getSizeInfo().getFixedSize().y, surface::getSizeInfo().getPercentageSize().y);;
			}
		}
	};
}