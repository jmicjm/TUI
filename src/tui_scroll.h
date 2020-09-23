/*this file contains following elements:
struct scroll_appearance_a - describes active/inactive scroll appearance, used by scroll_appearance
struct scroll_appearance - describes scroll appearance
struct scroll_button - widget that displays a scroll*/
#pragma once
#include "tui_surface.h"
#include "tui_appearance.h"
#include "tui_text_utils.h"
#include "tui_active_element.h"
#include "tui_input.h"

#include <cmath>

namespace tui
{
	struct scroll_appearance_a
	{
		symbol slider;
		symbol line;
		symbol prev_arrow;
		symbol next_arrow;

		scroll_appearance_a(DIRECTION direction = tui::DIRECTION::VERTICAL)
		{
			switch (direction)
			{
			case tui::DIRECTION::VERTICAL:
				*this = scroll_appearance_a(U'\x2551', U'\x2502', U'\x25B2', U'\x25BC');
				break;
			case tui::DIRECTION::HORIZONTAL:
				*this = scroll_appearance_a(U'\x2550', U'\x2500', U'\x25C4', U'\x25BA');
			}
		}
		scroll_appearance_a(symbol slider, symbol line, symbol prev_arrow, symbol next_arrow) 
			: slider(slider), line(line), prev_arrow(prev_arrow), next_arrow(next_arrow) {}

		void setColor(color Color)
		{
			slider.setColor(Color);
			line.setColor(Color);
			prev_arrow.setColor(Color);
			next_arrow.setColor(Color);
		}
	};

	struct scroll_appearance : appearance
	{
	protected:
		scroll_appearance_a m_active_appearance;
		scroll_appearance_a m_inactive_appearance;
	public:
		scroll_appearance(DIRECTION direction = tui::DIRECTION::VERTICAL) : m_active_appearance(direction), m_inactive_appearance(direction)
		{
			m_inactive_appearance.setColor(tui::COLOR::DARKGRAY);
		}
		scroll_appearance(scroll_appearance_a active, scroll_appearance_a inactive) : m_active_appearance(active), m_inactive_appearance(inactive) {}

		void setColor(color Color) override
		{
			m_active_appearance.setColor(Color);
			m_inactive_appearance.setColor(Color);
			setAppearanceAction();
		}

		void setAppearance(scroll_appearance appearance) { setElement(*this, appearance); }
		scroll_appearance getAppearance() const { return *this; }

		void setActiveAppearance(scroll_appearance_a active) { setElement(m_active_appearance, active); }
		scroll_appearance_a getActiveAppearance() const { return m_active_appearance; }

		void setInactiveAppearance(scroll_appearance_a inactive) { setElement(m_inactive_appearance, inactive); }
		scroll_appearance_a getInactiveAppearance() const { return m_inactive_appearance; }
	};

	template<DIRECTION direction>
	struct scroll : surface1D<direction>, scroll_appearance, active_element
	{
	private:
		int m_content_length = 0;
		int m_visible_content_length = -1;
		int m_top_position = 0;
		int m_current_position = 0;

		bool m_free_mode = false;

		bool m_redraw_needed = true;

		int visibleContentLength() const
		{
			if (m_visible_content_length < 0) { return surface1D<direction>::getSize(); }
			else { return m_visible_content_length; }
		}

		scroll_appearance_a gca() const
		{
			if (isActive()) { return m_active_appearance; }
			else { return m_inactive_appearance; }
		}

		void fill()
		{
			if (isNeeded())
			{
				surface1D<direction>::setSymbolAt(gca().prev_arrow, 0);
				surface1D<direction>::setSymbolAt(gca().next_arrow, surface1D<direction>::getSize() - 1);

				for (int i = 1; i < surface1D<direction>::getSize() -1; i++) { surface1D<direction>::setSymbolAt(gca().line, i); }

				int handle_length = ((visibleContentLength() * 1.f) / m_content_length) * (surface1D<direction>::getSize() - 2);

				if (handle_length < 1) { handle_length = 1; }

				float handle_pos_perc = m_top_position * 1.f / (m_content_length * 1.f - visibleContentLength());

				int handle_position = 0;
				
				if (surface1D<direction>::getSize() - 2 >= 0)
				{
					handle_position = round((surface1D<direction>::getSize() - 2) * (handle_pos_perc)-handle_length * (handle_pos_perc)) + 1;
				}

				for (int i = 0; i < handle_length; i++) 
				{
					surface1D<direction>::setSymbolAt(gca().slider, i + handle_position); 
				}
			}
			else
			{
				surface::makeTransparent();
			}
		}

		void adjustHandlePosition()
		{
			if (!isNeeded())
			{
				m_top_position = 0;
			}
			else if (m_top_position > m_content_length - visibleContentLength())
			{
				m_top_position = m_content_length - visibleContentLength();
			}	

			if (!m_free_mode)
			{
				m_current_position = m_top_position;
			}
			else
			{
				if (m_current_position < m_top_position)
				{
					m_top_position = m_current_position;
				}
				if (m_current_position >= m_content_length)
				{
					m_current_position = m_content_length - 1;
				}
				if (m_current_position >= m_top_position + visibleContentLength())
				{
					m_top_position = m_current_position - visibleContentLength() + 1;
				}
			}

			if (m_top_position < 0) { m_top_position = 0; }
			if (m_current_position < 0) { m_current_position = 0; }
		}

		void updateAction() override { update(); }
		void drawAction(surface::action_proxy proxy) override
		{
			if (m_redraw_needed) { fill(); }
			m_redraw_needed = false;
		}
		void resizeAction() override
		{
			adjustHandlePosition();
			m_redraw_needed = true;
		}

		void setAppearanceAction() override { m_redraw_needed = true; }

		void activationAction() override { m_redraw_needed = true; }
		void deactivationAction() override { m_redraw_needed = true; }

	public:
		short key_up = input::KEY::UP;
		short key_down = input::KEY::DOWN;
		short key_pgup = input::KEY::PGUP;
		short key_pgdn = input::KEY::PGDN;

		scroll(surface1D_size size = surface1D_size()) : scroll_appearance(direction)
		{
			if(direction == DIRECTION::HORIZONTAL)
			{
				key_up = input::KEY::LEFT;
				key_down = input::KEY::RIGHT;
			}

			surface1D<direction>::setSizeInfo(size);
		}

		bool isNeeded() const
		{
			return m_content_length > visibleContentLength();
		};

		void setContentLength(unsigned int length)
		{
			m_content_length = length; 

			adjustHandlePosition();
			m_redraw_needed = true;
		}
		unsigned int getContentLength() const { return m_content_length; }

		void setVisibleContentLength(int length)
		{
			m_visible_content_length = length;
			adjustHandlePosition();
			m_redraw_needed = true;
		}
		int getVisibleContentLength() const { return m_visible_content_length; }

		void useFreeMode(bool use)
		{
			m_free_mode = use;
			if (!use)
			{
				m_current_position = m_top_position;
			}
		}
		bool isUsingFreeMode() const { return m_free_mode; }

		void setTopPosition(unsigned int handle_position)
		{
			m_top_position = handle_position;
			m_current_position = handle_position;

			adjustHandlePosition();
			m_redraw_needed = true;
		}
		unsigned int getTopPosition() const { return m_top_position; }

		void setCurrentPosition(unsigned int handle_position)
		{
			m_current_position = handle_position;

			adjustHandlePosition();
			m_redraw_needed = true;
		}
		unsigned int getCurrentPosition() const { return m_current_position; }

		void up(unsigned int n = 1)
		{
			if (!m_free_mode)
			{
				setTopPosition(getTopPosition() - n);
			}
			else
			{
				m_current_position-=n;
				adjustHandlePosition();
			}
			m_redraw_needed = true;
		}
		void down(unsigned int n = 1)
		{
			if (!m_free_mode)
			{
				setTopPosition(getTopPosition() + n);
			}
			else
			{
				m_current_position+=n;
				adjustHandlePosition();
			}
			m_redraw_needed = true;
		}

		void pageUp(unsigned int n = 1)
		{
			up(n * visibleContentLength());
		}
		void pageDown(unsigned int n = 1)
		{
			down(n * visibleContentLength());
		}

		void update()
		{
			if (isActive()) 
			{
				if (input::isKeyPressed(key_up))   { up(); }
				if (input::isKeyPressed(key_down)) { down(); }
				if (input::isKeyPressed(key_pgup)) { pageUp(); }
				if (input::isKeyPressed(key_pgdn)) { pageDown(); }
			}
		}

	};
}