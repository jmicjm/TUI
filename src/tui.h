#pragma once

#include "tui_config.h"

#include "tui_console.h"
//#include "tui_input.h"
//#include "tui_utils.h"
//#include "tui_enums.h"
#include <string>
#include <iostream>
#include <cmath>



namespace tui 
{
	struct appearance
	{
	protected:
		virtual void setAppearance_action() {}
	public:
		virtual void setColor(color Color) = 0;
	};

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
		public:
			void resize_action() override { fill(); }
			void setAppearance_action() override { fill(); }
	};

	struct box_appearance : appearance
	{
	protected:
		symbol m_horizontal_line;
		symbol m_vertical_line;
		symbol m_top_left;
		symbol m_top_right;
		symbol m_bottom_left;
		symbol m_bottom_right;
	public:
		box_appearance() : box_appearance(U'\x2550', U'\x2551', U'\x2554', U'\x2557', U'\x255A', U'\x255D') {}
		box_appearance(symbol Symbol) : box_appearance(Symbol, Symbol, Symbol, Symbol, Symbol, Symbol) {}
		box_appearance(symbol h_line, symbol v_line, symbol top_l, symbol top_r, symbol bottom_l, symbol bottom_r) 
			: m_horizontal_line(h_line), m_vertical_line(v_line), m_top_left(top_l),
			m_top_right(top_r), m_bottom_left(bottom_l), m_bottom_right(bottom_r) {}

		void setColor(color Color) override
		{
			m_horizontal_line.setColor(Color);
			m_vertical_line.setColor(Color);
			m_top_left.setColor(Color);
			m_top_right.setColor(Color);
			m_bottom_left.setColor(Color);
			m_bottom_right.setColor(Color);
			setAppearance_action();
		}

		void setAppearance(box_appearance appearance)
		{
			*this = appearance;
			setAppearance_action();
		}
		box_appearance getAppearance() { return *this; }
		
		void setHorizontalLine(symbol line)
		{
			m_horizontal_line = line;
			setAppearance_action();
		}
		symbol getHorizontalLine() { return m_horizontal_line; }
		void setVerticalLine(symbol line)
		{
			m_vertical_line = line;
			setAppearance_action();
		}
		symbol getVerticalLine() { return m_vertical_line; }
		void setTopLeft(symbol top_l)
		{
			m_top_left = top_l;
			setAppearance_action();
		}
		symbol getTopLeft() { return m_top_left; }
		void setTopRight(symbol top_r)
		{
			m_top_right = top_r;
			setAppearance_action();
		}
		symbol getTopRight() { return m_top_right; }
		void setBottomLeft(symbol bottom_l)
		{
			m_bottom_left = bottom_l;
			setAppearance_action();
		}
		symbol getBottomLeft() { return m_bottom_left; }
		void setBottomRight(symbol bottom_r)
		{
			m_bottom_right = bottom_r;
			setAppearance_action();
		}
		symbol getBottomRight() { return m_bottom_right; }

	};
	
	struct box : surface, box_appearance
	{
		private:
			void fill()
			{
				setSymbolAt(m_top_left, vec2i(0, 0));
				setSymbolAt(m_top_right, vec2i(getSize().x - 1, 0));
				setSymbolAt(m_bottom_left, vec2i(0, getSize().y - 1));
				setSymbolAt(m_bottom_right, vec2i(getSize().x - 1, getSize().y - 1));

				for(int i = 1; i < getSize().x-1; i++) { setSymbolAt(m_horizontal_line, vec2i(i, 0)); }
				for(int i = 1; i < getSize().x-1; i++) { setSymbolAt(m_horizontal_line, vec2i(i, getSize().y - 1)); }

				for (int i = 1; i < getSize().y-1; i++) { setSymbolAt(m_vertical_line, vec2i(0, i)); }
				for (int i = 1; i < getSize().y-1; i++) { setSymbolAt(m_vertical_line, vec2i(getSize().x - 1, i)); }
			}
		public:
			box(surface_size size)
			{
				setSize(size);
				fill();
			}

			void resize_action() override { fill(); }	
			void setAppearance_action() override { fill(); }
	};


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
				
					m_handle_length = ((surface1D<direction>::getSize() * 1.f) / m_content_length)* surface1D<direction>::getSize();

					if (m_handle_length < 1) { m_handle_length = 1; }			

					float handle_pos_perc = m_handle_position * 1.f / (m_content_length*1.f - visibleContentLength());

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

				surface1D<direction>::setSize({size.fixed_size, size.percentage_size});
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

	struct text_appearance : appearance
	{
	protected:
		scroll_appearance m_active_scroll;
		scroll_appearance m_inactive_scroll;
	public:
		text_appearance() : text_appearance({ { U'\x2551', COLOR::WHITE }, { U'\x2502', COLOR::WHITE } },
											{ { U'\x2551', COLOR::DARKGRAY }, { U'\x2502', COLOR::DARKGRAY } }) {}
		text_appearance(scroll_appearance active, scroll_appearance inactive) : m_active_scroll(active), m_inactive_scroll(inactive) {}

		void setColor(color Color) override
		{
			m_active_scroll.setColor(Color);
			m_inactive_scroll.setColor(Color);
			setAppearance_action();
		}

		void setAppearance(text_appearance appearance)
		{
			*this = appearance;
			setAppearance_action();
		}
		text_appearance getAppearance() { return *this; }

		void setActiveScroll(scroll_appearance active)
		{
			m_active_scroll = active;
			setAppearance_action();
		}
		scroll_appearance getActiveScroll() { return m_active_scroll; }
		void setInactiveScroll(scroll_appearance inactive)
		{
			m_inactive_scroll = inactive;
			setAppearance_action();
		}
		scroll_appearance getInactiveScroll() { return m_inactive_scroll; }
	};

	struct text : surface, text_appearance, active_element
	{
	private:
		surface m_text;
		scroll<DIRECTION::VERTICAL> m_scroll;

		console_string m_unprepared_text;
		console_string m_prepared_text;

		bool m_use_prepared_text = true;
		bool m_use_dense_punctuation = false;
		bool m_use_control_characters = true;
		bool m_display_scroll = true;

		void fill()
		{
			m_text.makeTransparent();

			for (int i = 0; i < m_text.getSize().x; i++)
			{
				for (int j = 0; j < m_text.getSize().y; j++)
				{
					if (m_text.getSize().x * m_scroll.getHandlePosition() + j * m_text.getSize().x + i < m_prepared_text.size())
					{
						m_text[i][j] = m_prepared_text[m_text.getSize().x * m_scroll.getHandlePosition() + j * m_text.getSize().x + i];
					}
				}
			}

			makeTransparent();
			insertSurface(m_text);
			if (m_display_scroll && m_scroll.isNeeded()) { insertSurface(m_scroll); }
		}
		void prepareText()
		{
			console_string prepared;
			
			auto isControl = [](char32_t ch)
			{
				return GetGraphemeType(ch) == GRAPHEME_TYPE::CONTROL 
					|| GetGraphemeType(ch) == GRAPHEME_TYPE::CR 
					|| GetGraphemeType(ch) == GRAPHEME_TYPE::LF;
			};
			auto usePrepared = [&]()
			{
				return m_use_prepared_text && getSize().x > 2;
			};

			int pos = 0;
			for (int i=0;i < m_unprepared_text.size();i++)
			{	
				int pos_in_line = pos % m_text.getSize().x;

				if (isControl(m_unprepared_text[i].getFirstChar()))
				{
					if (m_use_control_characters)
					{
						if (m_unprepared_text[i].getFirstChar() == '\n')
						{
							for (int i = 0; i < (m_text.getSize().x - pos_in_line); i++)
							{
								prepared += " ";
							}
							pos += m_text.getSize().x - pos_in_line;	
						}
					}
					continue;
				}

				if (   usePrepared()
					&& pos_in_line == m_text.getSize().x - 1
					&& i+1 < m_unprepared_text.size()
					&& m_unprepared_text[i].getSymbol() != (U" ")	
					&& m_unprepared_text[i + 1].getSymbol() != (U" ")
					&& !isControl(m_unprepared_text[i + 1].getFirstChar())
					&& (m_use_dense_punctuation ? !IsPunctuation(m_unprepared_text[i]) : true)
					&& (m_use_dense_punctuation ? !IsPunctuation(m_unprepared_text[i + 1]) : true)
					)
				{
					prepared << m_unprepared_text[i - 1].getColor();
					if (m_unprepared_text[i - 1].getSymbol() != U" ") 
					{
						prepared << "-";	
					}
					else { prepared << " "; }
					pos++;
				}
				if (usePrepared() && pos_in_line == 0 && m_unprepared_text[i].getSymbol() == U" ") { continue; }//omit space at start of line

				prepared += m_unprepared_text[i];
				pos++;	
			}

			m_prepared_text = prepared;
		}

	public:
		int keyUp = KEYBOARD::KEY::UP;
		int keyDown = KEYBOARD::KEY::DOWN;
		int keyPageUp = KEYBOARD::KEY::PGUP;
		int keyPageDown = KEYBOARD::KEY::PGDN;

		text(surface_size size, console_string txt) : m_scroll({ 0, 100 })
			{
				setSize(size);
				m_text.setSize({ {-1,0}, {100,100} });

				m_scroll.setPosition(position({ 0,0 }, { 0,0 }, { POSITION::HORIZONTAL::RIGHT, POSITION::VERTICAL::TOP }));
				setText(txt);	

				setAppearance_action();
			}

			void adjustSizes()
			{
				m_text.setSize({ {0,0}, {100,100} });
				updateSurfaceSize(m_text);
				prepareText();
				if (m_display_scroll)
				{
					if (getNumberOfLines() > m_text.getSize().y)
					{
						m_text.setSize({ {-1,0}, {100,100} });
						updateSurfaceSize(m_text);
						prepareText();
					}
				}
				m_scroll.setContentLength(getNumberOfLines());
			}

			void setText(console_string txt)
			{
				m_unprepared_text = txt;
				adjustSizes();
				fill();
			}

			console_string getText() { return m_unprepared_text; }
			int getNumberOfLines()
			{
				return ceil(m_prepared_text.size() / (m_text.getSize().x * 1.f));
			}

			void goToLine(int line)
			{
				m_scroll.setHandlePosition(line);
				fill();
			}

			void lineUp()
			{
				m_scroll.setHandlePosition(m_scroll.getHandlePosition() - 1);
				fill();
			}
			void lineDown()
			{
				m_scroll.setHandlePosition(m_scroll.getHandlePosition() + 1);
				fill();
			}
			void pageUp()
			{
				m_scroll.setHandlePosition(m_scroll.getHandlePosition() - getSize().y);
				fill();
			}
			void pageDown()
			{
				m_scroll.setHandlePosition(m_scroll.getHandlePosition() + getSize().y);
				fill();
			}

			void usePreparedText(bool use)
			{
				m_use_prepared_text = use;
				adjustSizes();
				fill();
			}
			bool isUsingPreparedText() { return m_use_prepared_text; }

			void useDensePunctuation(bool use)
			{
				m_use_dense_punctuation = use;
				adjustSizes();
				fill();
			}
			bool isUsingDensePunctuation() { return m_use_dense_punctuation; }

			void useControlCharacters(bool use)
			{
				m_use_control_characters = use;
				adjustSizes();
				fill();
			}
			bool isUsingControlCharacters() { return m_use_control_characters; }

			void displayScroll(bool display)
			{
				m_display_scroll = display;
				adjustSizes();
				fill();
			}
			bool isDisplayingScroll() { return m_display_scroll; }

			void resizeToText(int max_width)
			{
				if (max_width <= 0)
				{
					m_use_control_characters = false;//temporarily disable control characters

					setSize({ {m_unprepared_text.size(), 1} });
					adjustSizes();
					/*
					with control characters disabled one line prepared text could be shorter than unprepared text
					e.g. space at start of line will be removed and all control characters will be removed
					*/
					setSize({ {m_prepared_text.size(), 1} });

					m_use_control_characters = true;
				}
				else
				{
					m_display_scroll = false;

					setSize({ {max_width, 1} });
					prepareText();

					setSize({ {max_width, (int)ceil( 1.f * m_prepared_text.size()/max_width)} });
					fill();

					m_display_scroll = true;
				}
			}
			void resizeToText() { resizeToText(0); }

			void update()
			{
				if (isActive()) {
					if (KEYBOARD::isKeyPressed(keyUp)) {
						lineUp();
					}
					if (KEYBOARD::isKeyPressed(keyDown)) {
						lineDown();
					}
					if (KEYBOARD::isKeyPressed(keyPageUp)) {
						pageUp();
					}
					if (KEYBOARD::isKeyPressed(keyPageDown)) {
						pageDown();
					}
				}
			}


			void draw_action() 
			{
				update();
			}
			void resize_action()
			{
				//m_scroll.setVisibleContentLength(getSize().y);
				updateSurfaceSize(m_scroll);
				adjustSizes();
				fill();
			}

			void activation_action() 
			{
				m_scroll.setAppearance(m_active_scroll);
				if (m_display_scroll && m_scroll.isNeeded()) { insertSurface(m_scroll); }
			}
			void disactivation_action() 
			{
				m_scroll.setAppearance(m_inactive_scroll);
				if (m_display_scroll && m_scroll.isNeeded()) { insertSurface(m_scroll); }
			}
			void setAppearance_action() override
			{
				if(isActive()){ m_scroll.setAppearance(m_active_scroll); }
				else { m_scroll.setAppearance(m_inactive_scroll); }
				if (m_display_scroll && m_scroll.isNeeded()) { insertSurface(m_scroll); }
			}

	};

	template<int direction>
	struct bar : surface
	{
	private:
		int m_length;
		int m_max_length;

		symbol m_bar;
		symbol m_start;
		symbol m_end;

		void fill()
		{
			
		}

	};


	

}

