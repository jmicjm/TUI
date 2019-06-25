#pragma once
#include <cmath>

#include "tui_console.h"
#include "tui_appearance.h"
#include "tui_text_utils.h"

namespace tui
{
	struct scroll_appearance : appearance
	{
	protected:
		symbol m_slider; //handle
		symbol m_line;
	public:
		scroll_appearance() : scroll_appearance(U'\x2551', U'\x2502') {}
		scroll_appearance(symbol slider, symbol line) : m_slider(slider), m_line(line) {}

		void setColor(color Color) override
		{
			m_slider.setColor(Color);
			m_line.setColor(Color);
			setAppearance_action();
		}

		void setAppearance(scroll_appearance appearance)
		{
			*this = appearance;
			setAppearance_action();
		}
		scroll_appearance getAppearance() { return *this; }

		void setSlider(symbol slider)
		{
			m_slider = slider;
			setAppearance_action();
		}
		symbol getSlider() { return m_slider; }
		void setLine(symbol line)
		{
			m_line = line;
			setAppearance_action();
		}
	};

	template<int direction>
	struct scroll : surface1D<direction>, scroll_appearance
	{
	private:
		int m_content_length = 0;
		int m_visible_content_length = -1;
		int m_handle_position = 0;
		int m_handle_length = 0;

		int visibleContentLength()
		{
			if (m_visible_content_length < 0) { return surface1D<direction>::getSize(); }
			else { return m_visible_content_length; }
		}

		void fill()
		{
			if (isNeeded())
			{
				for (int i = 0; i < surface1D<direction>::getSize(); i++) { surface1D<direction>::setSymbolAt(m_line, i); }

				m_handle_length = ((surface1D<direction>::getSize() * 1.f) / m_content_length) * surface1D<direction>::getSize();

				if (m_handle_length < 1) { m_handle_length = 1; }

				float handle_pos_perc = m_handle_position * 1.f / (m_content_length * 1.f - visibleContentLength());

				int handle_position = round(surface1D<direction>::getSize() * (handle_pos_perc)-m_handle_length * (handle_pos_perc));

				for (int i = 0; i < m_handle_length; i++) { surface1D<direction>::setSymbolAt(m_slider, i + handle_position); }
			}
			else
			{
				surface::makeTransparent();
			}
		}
	public:
		scroll(surface1D_size size)
		{
			switch (direction)
			{
			case DIRECTION::HORIZONTAL:
				m_slider = U'\x2550';
				m_line = U'\x2500';
				break;
			case DIRECTION::VERTICAL:
				m_slider = U'\x2551';
				m_line = U'\x2502';
				break;
			}

			surface1D<direction>::setSize({ size.fixed_size, size.percentage_size });
		}

		/*void setChars(symbol slider, symbol line)
		{
			m_slider = slider;
			m_line = line;
			fill();
		}
		void setColors(color slider, color line)
		{
			m_slider.setColor(slider);
			m_line.setColor(line);
		}*/

		bool isNeeded()
		{
			return m_content_length > visibleContentLength();
		};

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


		void update()
		{
			//fill(); 
		}

		void draw_action() override { fill(); }
		void resize_action() override
		{
			adjustHandlePositionRespectLength();
			fill();
		}
		void setAppearance_action() override { fill(); }

	};
}