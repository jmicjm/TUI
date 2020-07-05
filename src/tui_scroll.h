#pragma once
#include <cmath>

#include "tui_surface.h"
#include "tui_appearance.h"
#include "tui_text_utils.h"
#include "tui_active_element.h"

namespace tui
{
	struct scroll_appearance_a
	{
		symbol slider; //handle
		symbol line;
		symbol prev_arrow;
		symbol next_arrow;

		scroll_appearance_a(DIRECTION direction = tui::DIRECTION::VERTICAL)
		{
			switch (direction)
			{
			case tui::DIRECTION::VERTICAL:
				slider = U'\x2551';
				line = U'\x2502';
				prev_arrow = U'\x25B2';
				next_arrow = U'\x25BC';
				break;
			case tui::DIRECTION::HORIZONTAL:
				slider = U'\x2550';
				line = U'\x2500';
				prev_arrow = U'\x25C4';
				next_arrow = U'\x25BA';
			}
		}
		scroll_appearance_a(symbol slider, symbol line) : slider(slider), line(line) {}

		void setColor(color Color)
		{
			slider.setColor(Color);
			line.setColor(Color);
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
		scroll_appearance getAppearance() { return *this; }

		void setActiveAppearance(scroll_appearance_a active) { setElement(m_active_appearance, active); }
		scroll_appearance_a getActiveAppearance() { return m_active_appearance; }

		void setInactiveAppearance(scroll_appearance_a inactive) { setElement(m_inactive_appearance, inactive); }
		scroll_appearance_a getInactiveAppearance() { return m_inactive_appearance; }
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

		bool m_immobilized = false;

		int visibleContentLength()
		{
			if (m_visible_content_length < 0) { return surface1D<direction>::getSize(); }
			else { return m_visible_content_length; }
		}

		scroll_appearance_a getCurrentAppearance()
		{
			if (isActive()) { return m_active_appearance; }
			else { return m_inactive_appearance; }
		}

		void fill()
		{
			if (isNeeded())
			{
				surface1D<direction>::setSymbolAt(getCurrentAppearance().prev_arrow, 0);
				surface1D<direction>::setSymbolAt(getCurrentAppearance().next_arrow, surface1D<direction>::getSize() - 1);

				for (int i = 1; i < surface1D<direction>::getSize() -1; i++) { surface1D<direction>::setSymbolAt(getCurrentAppearance().line, i); }

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
					surface1D<direction>::setSymbolAt(getCurrentAppearance().slider, i + handle_position); 
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
			else if (m_current_position >= m_top_position + visibleContentLength())
			{
				m_current_position = m_top_position + visibleContentLength() - 1;
			}

			if (m_top_position < 0) { m_top_position = 0; }
			if (m_current_position < 0) { m_current_position = 0; }
		}

		void updateAction() override { update(); }
		void drawAction() override { fill(); }
		void resizeAction() override
		{
			adjustHandlePosition();
			fill();
		}

		void setAppearanceAction() override { fill(); }

		void activationAction() override { fill(); }
		void disactivationAction() override { fill(); }

	public:
		int key_up = input::KEY::UP;
		int key_down = input::KEY::DOWN;
		int key_pgup = input::KEY::PGUP;
		int key_pgdn = input::KEY::PGDN;

		scroll() : scroll(1) {}
		scroll(surface1D_size size) : scroll_appearance(direction)
		{
			if(direction == DIRECTION::HORIZONTAL)
			{
				key_up = input::KEY::LEFT;
				key_down = input::KEY::RIGHT;
			}

			surface1D<direction>::setSizeInfo({ size.fixed, size.percentage });
		}

		bool isNeeded()
		{
			return m_content_length > visibleContentLength();
		};

		void setContentLength(int length)
		{
			if (length >= 0) { m_content_length = length; }
			else { m_content_length = 0; }

			adjustHandlePosition();
			fill();
		}
		void setVisibleContentLength(int length)
		{
			m_visible_content_length = length;
			adjustHandlePosition();
			fill();
		}
		int getVisibleContentLength() { return m_visible_content_length; }

		void useFreeMode(bool use)
		{
			m_free_mode = use;
			if (!use)
			{
				m_current_position = m_top_position;
			}
		}
		bool isUsingFreeMode() { return m_free_mode; }

		void setTopPosition(int handle_position)
		{
			m_top_position = handle_position;
			m_current_position = handle_position;

			adjustHandlePosition();
			fill();
		}

		int getContentLength() { return m_content_length; }
		int getTopPosition() { return m_top_position; }
		int getCurrentPosition() { return m_current_position; }

		void immobilize(bool i) { m_immobilized = i; }
		bool isImmobilized() { return m_immobilized; }


		void up(unsigned int n = 1)
		{
			if (!m_free_mode)
			{
				setTopPosition(getTopPosition() - n);
			}
			else
			{
				m_current_position--;
				if (m_current_position < m_top_position)
				{
					m_top_position = m_current_position;
				}
				adjustHandlePosition();
			}
		}
		void down(unsigned int n = 1)
		{
			if (!m_free_mode)
			{
				setTopPosition(getTopPosition() + n);
			}
			else
			{
				m_current_position++;
				if (m_current_position >= m_top_position + visibleContentLength())
				{
					m_top_position = m_current_position - visibleContentLength() + 1;
				}
				adjustHandlePosition();
			}
		}
		void pageUp(unsigned int n = 1)
		{
			setTopPosition(getTopPosition() - n * visibleContentLength());
		}
		void pageDown(unsigned int n = 1)
		{
			setTopPosition(getTopPosition() + n * visibleContentLength());
		}

		void update()
		{
			if (isActive() && !isImmobilized()) {
				if (input::isKeyPressed(key_up)) { up(); }
				if (input::isKeyPressed(key_down)) { down(); }
				if (input::isKeyPressed(key_pgup)) { pageUp(); }
				if (input::isKeyPressed(key_pgdn)) { pageDown(); }
			}
		}

	};
}