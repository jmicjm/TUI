#pragma once
#include "tui_utils.h"
#include "tui_text_utils.h"

#include <vector>
#include <cmath>

namespace tui
{
	struct surface_size
	{
		vec2i fixed;
		vec2f percentage;

		surface_size() : surface_size({ 0,0 }, { 0,0 }) {}
		surface_size(vec2i Fixed) : surface_size(Fixed, { 0,0 }) {}
		surface_size(vec2i Fixed, vec2f Percentage) : fixed(Fixed), percentage(Percentage) {}
	};

	struct surface1D_size
	{
		int fixed;
		float percentage;

		surface1D_size() : surface1D_size(0, 0) {}
		surface1D_size(int Fixed) : surface1D_size(Fixed, 0) {}
		surface1D_size(int Fixed, float Percentage) : fixed(Fixed), percentage(Percentage) {}
	};

	namespace POSITION
	{
		enum POSITION
		{
			BEGIN = 0, CENTER = 50, END = 100
		};
	}

	struct position
	{
		vec2i offset;
		vec2f percentage_offset;
		vec2f relative;

		position() : position({ 0, 0 }, { 0, 0 }, { POSITION::BEGIN, POSITION::BEGIN }) {}
		position(vec2i offset) : position(offset, { 0, 0 }, { POSITION::BEGIN, POSITION::BEGIN }) {}
		position(vec2i offset, vec2f percentage_offset) : position(offset, percentage_offset, { POSITION::BEGIN, POSITION::BEGIN }) {}
		position(vec2i Offset, vec2f Percentage_offset, vec2f Relative) : offset(Offset), percentage_offset(Percentage_offset), relative(Relative) {}
	};

	struct surface
	{
	private:
		std::vector<symbol> m_symbols;
		unsigned int m_width = 1;
		position m_position_info;
		vec2i m_position;
		vec2i m_global_position;
		surface_size m_size_info;

		bool m_resized;

		void resize(vec2i size)
		{
			m_resized = size != getSize();

			if (m_resized)
			{
				vec2i new_size = size;
				if (size.x < 1) { new_size.x = 1; }
				if (size.y < 1) { new_size.y = 1; }

				m_symbols.resize(new_size.x * new_size.y);
				m_width = new_size.x;

				makeTransparent();
				resizeAction();
			}
		}

		void setString(const symbol_string& str)
		{
			setSizeInfo({ {(int)str.size(),1},{0,0} });

			for (int i = 0; i < getSize().x; i++)
			{
				setSymbolAt(str[i], { i,0 });
			}
		}

		template<typename T>
		struct surface_proxy
		{
			friend class surface;
		private:
			T* surf;
			size_t x;
			surface_proxy(T* s, size_t x) : surf(s), x(x) {}
		public:
			symbol& operator[](size_t y)
			{
				return surf->m_symbols[surf->m_width * y + x];
			}
			const symbol& operator[](size_t y) const
			{
				return surf->m_symbols[surf->m_width * y + x];
			}
		};

	protected:
		virtual void resizeAction() {}
		virtual void updateAction() {}
		virtual void drawAction() {}
	public:
		surface(surface_size size = {{ 1,1 }, { 0,0 }})
		{
			setSizeInfo(size);
		}
		surface(const symbol& sym) : surface(symbol_string(sym)) {}
		surface(const symbol_string& str)
		{
			setString(str);
		}
		virtual ~surface() {}

		surface& operator=(const symbol_string& str)
		{
			setString(str);
			return *this;
		}

		surface_proxy<surface> operator[](size_t x)
		{
			return surface_proxy<surface>(this, x);
		}
		const surface_proxy<const surface> operator[](size_t x) const 
		{
			return surface_proxy<const surface>(this, x);
		}

		bool isResized() const { return m_resized; }

		void setSymbolAt(const symbol& character, vec2i position) { m_symbols[position.y * m_width + position.x] = character; }
		symbol getSymbolAt(vec2i position) const { return m_symbols[position.y * m_width + position.x]; }

		void move(vec2i offset)
		{
			m_position_info.offset += offset;
		}

		void setPositionInfo(position pos) { m_position_info = pos; }

		void setSizeInfo(surface_size size)
		{
			m_size_info = size;
			resize(size.fixed);
		}

		void updateSurfaceSize(surface& surf) const
		{
			if (&surf != this)
			{	
				vec2i int_size = surf.getSizeInfo().fixed;
				vec2f perc_size = surf.getSizeInfo().percentage;

				int x = std::round((perc_size.x / 100.f) * getSize().x + int_size.x);
				int y = std::round((perc_size.y / 100.f) * getSize().y + int_size.y);

				surf.resize({ x,y });
			}
		}

		void updateSurfacePosition(surface& surf) const
		{
			if (&surf != this)
			{
				vec2f origin;

				origin.x = getSize().x * (surf.getPositionInfo().relative.x / 100.f) - surf.getSize().x * (surf.getPositionInfo().relative.x / 100.f)
					+ surf.getPositionInfo().offset.x 
					+ surf.getPositionInfo().percentage_offset.x * getSize().x / 100.f;

				origin.y = getSize().y * (surf.getPositionInfo().relative.y / 100.f) - surf.getSize().y * (surf.getPositionInfo().relative.y / 100.f)
					+ surf.getPositionInfo().offset.y
					+ surf.getPositionInfo().percentage_offset.y * getSize().y / 100.f;

				origin.x = std::round(origin.x);
				origin.y = std::round(origin.y);

				surf.m_position = origin;
				surf.m_global_position = m_global_position + origin;
			}
		}

		void insertSurface(surface& surf, bool update = true)
		{
			if (&surf != this)
			{
				updateSurfaceSize(surf);
				updateSurfacePosition(surf);

				if (update) { surf.updateAction(); }
				surf.drawAction();

				vec2i origin = surf.m_position;
				for (int y = 0; y < surf.getSize().y; y++)
				{
					for (int x = 0; x < surf.getSize().x; x++)
					{
						if (surf[x][y][0] != 0 //transparent
							&& origin.x + x >= 0
							&& origin.y + y >= 0
							&& origin.x + x < getSize().x
							&& origin.y + y < getSize().y)
						{
							setSymbolAt(surf[x][y], { origin.x + x, origin.y + y });
						}
					}
				}
			}
		}

		void fill(const symbol& Symbol)
		{
			for (int i = 0; i < m_symbols.size(); i++) { m_symbols[i] = Symbol; }
		}
		void makeTransparent() { fill((char32_t)0); }
		void makeBlank() { fill(' '); }

		void invert()
		{
			for (int i = 0; i < m_symbols.size(); i++) { m_symbols[i].invert(); }
		}

		vec2i getPosition() const { return m_position; }
		vec2i getGlobalPosition() const { return m_global_position; }
		position getPositionInfo() const { return m_position_info; }

		vec2i getSize() const { return vec2i(m_width, m_symbols.size() / m_width); }
		surface_size getSizeInfo() const { return m_size_info; }
	};

	enum class DIRECTION : bool
	{
		HORIZONTAL = 0,
		VERTICAL
	};
	
	template<DIRECTION direction>
	struct surface1D : surface
	{
		void setSizeInfo(surface1D_size size)
		{
			switch (direction)
			{
			case tui::DIRECTION::HORIZONTAL:
				surface::setSizeInfo({ {size.fixed, 1},{size.percentage, 0} });
				break;
			case tui::DIRECTION::VERTICAL:
				surface::setSizeInfo({ {1, size.fixed},{0, size.percentage} });
			}
		}
		int getSize() const
		{
			switch (direction)
			{
			case tui::DIRECTION::HORIZONTAL:
				return surface::getSize().x;
			case tui::DIRECTION::VERTICAL:
				return surface::getSize().y;
			}
		}
		void setSymbolAt(const symbol& character, int position)
		{
			switch (direction)
			{
			case tui::DIRECTION::HORIZONTAL:
				surface::setSymbolAt(character, { position, 0 });
				break;
			case tui::DIRECTION::VERTICAL:
				surface::setSymbolAt(character, { 0, position });
			}
		}
		symbol getSymbolAt(int position) const
		{
			switch (direction)
			{
			case tui::DIRECTION::HORIZONTAL:
				return surface::getSymbolAt({ position, 0 });
			case tui::DIRECTION::VERTICAL:
				return surface::getSymbolAt({ 0, position });
			}
		}
		symbol& operator[](size_t i)
		{
			switch (direction)
			{
			case tui::DIRECTION::HORIZONTAL:
				return surface::operator[](i)[0];
			case tui::DIRECTION::VERTICAL:
				return surface::operator[](0)[i];
			}
		}
		const symbol& operator[](size_t i) const 
		{
			switch (direction)
			{
			case tui::DIRECTION::HORIZONTAL:
				return surface::operator[](i)[0];
			case tui::DIRECTION::VERTICAL:
				return surface::operator[](0)[i];
			}
		}
		surface1D_size getSizeInfo() const
		{
			switch (direction)
			{
			case tui::DIRECTION::HORIZONTAL:
				return surface1D_size(surface::getSizeInfo().fixed.x, surface::getSizeInfo().percentage.x);
			case tui::DIRECTION::VERTICAL:
				return surface1D_size(surface::getSizeInfo().fixed.y, surface::getSizeInfo().percentage.y);;
			}
		}
	};
}