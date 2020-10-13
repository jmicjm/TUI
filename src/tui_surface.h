/*this file contains following elements:
struct surface_size - describes size of surface
struct surface1D_size - describes size of surface1D<DIRECTION>
struct surface_position - describes position of surface
struct anchor_position - describes position of anchor
struct surface - surface, could be viewed as array of symbols
struct surface1D<DIRECTION> - acts as an overlay on top of surface
enums: POSITION, SIDE, DIRECTION
*/
#pragma once
#include "tui_vec2.h"
#include "tui_text_utils.h"

#include <vector>
#include <cmath>
#include <functional>

namespace tui
{
	struct surface_size
	{
		vec2i fixed;
		vec2f percentage;

		surface_size(vec2i fixed = { 0,0 }, vec2f percentage = { 0,0 }) : fixed(fixed), percentage(percentage) {}
	};

	struct surface1D_size
	{
		int fixed;
		float percentage;

		surface1D_size(int fixed = 0, float percentage = 0) : fixed(fixed), percentage(percentage) {}
	};

	namespace POSITION
	{
		enum POSITION
		{
			BEGIN = 0, CENTER = 50, END = 100
		};
	}

	struct surface_position
	{
		vec2i offset;
		vec2f percentage_offset;
		vec2f relative;

		surface_position(vec2i offset = { 0,0 }, vec2f percentage_offset = { 0,0 }, vec2f relative = { POSITION::BEGIN, POSITION::BEGIN })
			: offset(offset), percentage_offset(percentage_offset), relative(relative) {}
	};

	enum class SIDE
	{
		TOP, BOTTOM, LEFT, RIGHT
	};

	struct anchor_position
	{
		SIDE side;
		float position;
		vec2i offset;

		anchor_position(SIDE side = SIDE::RIGHT, float position = POSITION::CENTER, vec2i offset = { 0,0 }) 
			: side(side), position(position), offset(offset) {}
	};

	struct surface
	{
	private:
		std::vector<symbol> m_symbols;
		unsigned int m_width = 1;
		surface_position m_position_info;
		vec2i m_position;
		vec2i m_global_position;
		surface_size m_size_info;

		surface* m_anchor = nullptr;
		anchor_position m_anchor_position_info;

		symbol m_clear_symbol = U'\0';

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

				clear();
				resizeAction();
			}
		}

		void setString(const symbol_string& str)
		{
			symbol_string s = getFullWidthString(str);

			setSizeInfo({ {(int)s.size(),1},{0,0} });

			for (int i = 0; i < s.size(); i++)
			{
				setSymbolAt(s[i], { i,0 });
			}
		}

		template<typename T>
		struct array_proxy
		{
			friend class surface;
		private:
			T* surf;
			size_t x;
			array_proxy(T* s, size_t x) : surf(s), x(x) {}
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
		struct action_proxy
		{
		private:
			surface* surf;
		public:
			action_proxy(surface* surf) : surf(surf) {}
			vec2i getSize() const { return surf->getSize(); }
			void updateSurfaceSize(surface& s) { surf->updateSurfaceSize(s); }
			void updateSurfacePosition(surface& s) { surf->updateSurfacePosition(s); }
			void insertSurface(surface& s, bool update = true) { surf->insertSurface(s, update); }
		};
		virtual void resizeAction() {}
		virtual void updateAction() {}
		virtual void drawAction(action_proxy proxy) {}

		void clear()
		{
			fill(m_clear_symbol);
		}
	public:
		surface(surface_size size = {{ 1,1 }, { 0,0 }})
		{
			setSizeInfo(size);
		}
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

		array_proxy<surface> operator[](size_t x)
		{
			return array_proxy<surface>(this, x);
		}
		const array_proxy<const surface> operator[](size_t x) const
		{
			return array_proxy<const surface>(this, x);
		}

		bool isResized() const { return m_resized; }

		void setSymbolAt(const symbol& character, vec2i position) { m_symbols[position.y * m_width + position.x] = character; }
		symbol getSymbolAt(vec2i position) const { return m_symbols[position.y * m_width + position.x]; }

		void move(vec2i offset)
		{
			m_position_info.offset += offset;
		}

		void setPositionInfo(surface_position pos) { m_position_info = pos; }
		surface_position getPositionInfo() const { return m_position_info; }

		vec2i getPosition() const { return m_position; }
		vec2i getGlobalPosition() const { return m_global_position; }

		void setAnchor(surface* surf) 
		{
			if (surf != this)
			{
				m_anchor = surf;
			}
		}
		surface* getAnchor() const { return m_anchor; }

		void setAnchorPositionInfo(anchor_position anchor_pos) { m_anchor_position_info = anchor_pos; }
		anchor_position getAnchorPositionInfo() const { return m_anchor_position_info; }

		void setSizeInfo(surface_size size, bool update_fixed = true)
		{
			m_size_info = size;
			if (size.percentage == vec2f(0.f, 0.f) && update_fixed)
			{
				resize(size.fixed);
			}
		}
		surface_size getSizeInfo() const { return m_size_info; }

		vec2i getSize() const { return vec2i(m_width, m_symbols.size() / m_width); }

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

				switch (surf.m_anchor == nullptr)
				{
				case true:
					origin.x = getSize().x * (surf.getPositionInfo().relative.x / 100.f) - surf.getSize().x * (surf.getPositionInfo().relative.x / 100.f)
						+ surf.getPositionInfo().offset.x
						+ surf.getPositionInfo().percentage_offset.x * getSize().x / 100.f;

					origin.y = getSize().y * (surf.getPositionInfo().relative.y / 100.f) - surf.getSize().y * (surf.getPositionInfo().relative.y / 100.f)
						+ surf.getPositionInfo().offset.y
						+ surf.getPositionInfo().percentage_offset.y * getSize().y / 100.f;
					break;

				case false:
					origin = surf.m_anchor->getPosition();

					switch (surf.m_anchor_position_info.side)
					{
					case SIDE::TOP:
					case SIDE::BOTTOM:
						origin.x += surf.m_anchor->getSize().x * (surf.m_anchor_position_info.position / 100.f) - surf.getSize().x * (surf.m_anchor_position_info.position / 100.f);
						break;	
					case SIDE::LEFT:
					case SIDE::RIGHT:
						origin.y += surf.m_anchor->getSize().y * (surf.m_anchor_position_info.position / 100.f) - surf.getSize().y * (surf.m_anchor_position_info.position / 100.f);
					}

					switch (surf.m_anchor_position_info.side)
					{
					case SIDE::TOP:
						origin.y -= surf.getSize().y;
						break;
					case SIDE::BOTTOM:
						origin.y += surf.m_anchor->getSize().y;
						break;
					case SIDE::LEFT:
						origin.x -= surf.getSize().x;
						break;
					case SIDE::RIGHT:
						origin.x += surf.m_anchor->getSize().x;
					}

					origin += surf.m_anchor_position_info.offset;
				}

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
				surf.drawAction(this);

				const vec2i origin = surf.m_position;
				for (int y = 0; y < surf.getSize().y; y++)
				{
					for (int x = 0; x < surf.getSize().x; x++)
					{
						color c = surf[x][y].getColor();

						switch (surf[x][y].getBgTransparencySrc())
						{
						case TRANSPARENCY_SRC::NONE:
							break;
						case TRANSPARENCY_SRC::BG:
							c.background = (*this)[origin.x + x][origin.y + y].getColor().background;
							break;
						case TRANSPARENCY_SRC::FG:
							c.background = (*this)[origin.x + x][origin.y + y].getColor().foreground;
							break;
						case TRANSPARENCY_SRC::AVG:
							rgb fg = (*this)[origin.x + x][origin.y + y].getColor().foreground;
							rgb bg = (*this)[origin.x + x][origin.y + y].getColor().background;
							c.background = { (uint8_t)((fg.r + bg.r)/2), (uint8_t)((fg.g + bg.g)/2), (uint8_t)((fg.b + bg.b)/2) };
						}

						if (surf[x][y][0] != 0 //fully transparent
							&& origin.x + x >= 0
							&& origin.y + y >= 0
							&& origin.x + x < getSize().x
							&& origin.y + y < getSize().y)
						{
							setSymbolAt(surf[x][y], { origin.x + x, origin.y + y });
							(*this)[origin.x + x][origin.y + y].setColor(c);
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

		void setClearSymbol(const symbol& sym) { m_clear_symbol = sym; }
		symbol getClearSymbol() const { return m_clear_symbol; }

		void invert()
		{
			for (int i = 0; i < m_symbols.size(); i++) { m_symbols[i].invert(); }
		}
	};

	enum class DIRECTION : bool
	{
		HORIZONTAL = 0,
		VERTICAL
	};
	
	template<DIRECTION direction>
	struct surface1D : surface
	{
		surface1D(surface1D_size size = { 1, 0 })
		{
			setSizeInfo(size);
		}

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