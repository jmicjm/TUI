#pragma once
#include <cmath>

#include "tui_console.h"
#include "tui_appearance.h"
#include "tui_text_utils.h"

namespace tui
{
	struct scroll_keys
	{
		int keyUp = KEYBOARD::KEY::UP;
		int keyDown = KEYBOARD::KEY::DOWN;
		int keyPageUp = KEYBOARD::KEY::PGUP;
		int keyPageDown = KEYBOARD::KEY::PGDN;
	};

	struct scroll_appearance_a
	{
		symbol slider; //handle
		symbol line;

		scroll_appearance_a() : scroll_appearance_a(U'\x2551', U'\x2502') {}
		scroll_appearance_a(symbol slider, symbol line) : slider(slider), line(line) {}

		void setColor(color Color)
		{
			slider.setColor(Color);
			line.setColor(Color);
		}

		void setAppearance(scroll_appearance_a appearance)
		{
			*this = appearance;
		}
		scroll_appearance_a getAppearance() { return *this; }

		void setSlider(symbol Slider)
		{
			slider = Slider;
		}
		symbol getSlider() { return slider; }
		void setLine(symbol Line)
		{
			line = Line;
		}
		symbol getLine() { return line; }
	};

	struct scroll_appearance : appearance
	{
	protected:
		scroll_appearance_a m_active_appearance;
		scroll_appearance_a m_inactive_appearance;
	public:
		scroll_appearance() : scroll_appearance({ { U'\x2551', COLOR::WHITE }, { U'\x2502', COLOR::WHITE } },
												{ { U'\x2551', COLOR::DARKGRAY }, { U'\x2502', COLOR::DARKGRAY } }) {}
		scroll_appearance(scroll_appearance_a active, scroll_appearance_a inactive) : m_active_appearance(active), m_inactive_appearance(inactive) {}

		void setColor(color Color) override
		{
			m_active_appearance.setColor(Color);
			m_inactive_appearance.setColor(Color);
			setAppearanceAction();
		}

		void setAppearance(scroll_appearance appearance)
		{
			*this = appearance;
			setAppearanceAction();
		}
		scroll_appearance getAppearance() { return *this; }

		void setActiveAppearance(scroll_appearance_a active)
		{
			m_active_appearance = active;
			setAppearanceAction();
		}
		scroll_appearance_a getActiveAppearance() { return m_active_appearance; }
		void setInactiveAppearance(scroll_appearance_a inactive)
		{
			m_inactive_appearance = inactive;
			setAppearanceAction();
		}
		scroll_appearance_a getInactiveAppearance() { return m_inactive_appearance; }
	};

	template<int direction>
	struct scroll : surface1D<direction>, scroll_appearance, active_element
	{
	private:
		int m_content_length = 0;
		int m_visible_content_length = -1;
		int m_handle_position = 0;
		int m_handle_length = 0;

		bool m_immobilized = false;

		scroll_keys keys;

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
				for (int i = 0; i < surface1D<direction>::getSize(); i++) { surface1D<direction>::setSymbolAt(getCurrentAppearance().line, i); }

				m_handle_length = ((surface1D<direction>::getSize() * 1.f) / m_content_length) * surface1D<direction>::getSize();

				if (m_handle_length < 1) { m_handle_length = 1; }

				float handle_pos_perc = m_handle_position * 1.f / (m_content_length * 1.f - visibleContentLength());

				int handle_position = round(surface1D<direction>::getSize() * (handle_pos_perc)-m_handle_length * (handle_pos_perc));

				for (int i = 0; i < m_handle_length; i++) { surface1D<direction>::setSymbolAt(getCurrentAppearance().slider, i + handle_position); }
			}
			else
			{
				surface::makeTransparent();
			}
		}

		void adjustHandlePositionRespectLength()
		{
			if (!isNeeded())
			{
				m_handle_position = 0;
			}
			else if (m_handle_position > m_content_length - visibleContentLength())
			{
				m_handle_position = m_content_length - visibleContentLength();
			}
		}

		void adjustHandlePositionRespectBounds()
		{
			adjustHandlePositionRespectLength();

			if (m_handle_position < 0) { m_handle_position = 0; }
		}

		void updateAction() override { update(); }
		void drawAction() override { fill(); }
		void resizeAction() override
		{
			adjustHandlePositionRespectLength();
			fill();
		}

		void setAppearanceAction() override { fill(); }

		void activationAction() override { fill(); }
		void disactivationAction() override { fill(); }

	public:
		scroll() : scroll(1) {}
		scroll(surface1D_size size)
		{
			if(direction == DIRECTION::HORIZONTAL)
			{ 
				setAppearance({ { { U'\x2550', COLOR::WHITE }, { U'\x2500', COLOR::WHITE } },
								{ { U'\x2550', COLOR::DARKGRAY }, { U'\x2500', COLOR::DARKGRAY } } });
				keys.keyUp = KEYBOARD::KEY::LEFT;
				keys.keyDown = KEYBOARD::KEY::RIGHT;
			}

			surface1D<direction>::setSize({ size.fixed_size, size.percentage_size });
		}

		bool isNeeded()
		{
			return m_content_length > visibleContentLength();
		};

		void setContentLength(int length)
		{
			if (length >= 0) { m_content_length = length; }
			else { m_content_length = 0; }

			adjustHandlePositionRespectLength();
			fill();
		}
		void setVisibleContentLength(int length)
		{
			m_visible_content_length = length;
			fill();
		}
		int getVisibleContentLength() { return m_visible_content_length; }

		void setHandlePosition(int handle_position)
		{
			m_handle_position = handle_position;

			adjustHandlePositionRespectBounds();
			fill();
		}

		int getContentLength() { return m_content_length; }
		int getHandlePosition() { return m_handle_position; }

		void immobilize(bool i) { m_immobilized = i; }
		bool isImmobilized() { return m_immobilized; }

		void setKeys(scroll_keys Keys) { keys = Keys; }
		scroll_keys getKeys() { return keys; }

		void update()
		{
			if (isActive() && !isImmobilized()) {
				if (KEYBOARD::isKeyPressed(keys.keyUp)) {
					setHandlePosition(getHandlePosition() - 1);
				}
				if (KEYBOARD::isKeyPressed(keys.keyDown)) {
					setHandlePosition(getHandlePosition() + 1);
				}
				if (KEYBOARD::isKeyPressed(keys.keyPageUp)) {
					setHandlePosition(getHandlePosition() - visibleContentLength());
				}
				if (KEYBOARD::isKeyPressed(keys.keyPageDown)) {
					setHandlePosition(getHandlePosition() + visibleContentLength());
				}
			}
		}

	};
}