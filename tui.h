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
	struct rectangle : surface, active_element
	{
		private:
			symbol m_character;

			void fill()
			{

				for (int i = 0; i < getSize().x; i++)
				{
					for (int j = 0; j < getSize().y; j++) {
						surface::setSymbolAt(m_character, vec2i(i, j));
					}
				}
			}
		public:
			void setChar(symbol character)
			{
				m_character = character;	
				fill();
			}
			void resize_action() { fill(); }
			void activation_action()
			{
				m_character.setColor(COLOR::GREEN);
				fill();
				
			}
			void disactivation_action()
			{
				m_character.setColor(COLOR::RED);
				fill();
			}
	};
	
	struct box : surface
	{
		private:
			symbol horizontal_line;
			symbol vertical_line;
			symbol top_left;
			symbol top_right;
			symbol bottom_left;
			symbol bottom_right;

			void fillChars()
			{
				surface::setSymbolAt(top_left, vec2i(0, 0));
				surface::setSymbolAt(top_right, vec2i(getSize().x - 1, 0));
				surface::setSymbolAt(bottom_left, vec2i(0, getSize().y - 1));
				surface::setSymbolAt(bottom_right, vec2i(getSize().x - 1, getSize().y - 1));

				for(int i = 1; i < getSize().x-1; i++) { surface::setSymbolAt(horizontal_line, vec2i(i, 0)); }
				for(int i = 1; i < getSize().x-1; i++) { surface::setSymbolAt(horizontal_line, vec2i(i, getSize().y - 1)); }

				for (int i = 1; i < getSize().y-1; i++) { surface::setSymbolAt(vertical_line, vec2i(0, i)); }
				for (int i = 1; i < getSize().y-1; i++) { surface::setSymbolAt(vertical_line, vec2i(getSize().x - 1, i)); }
			}
		public:
			box(surface_size size, int thickness)
			{
				setSize(size);
				switch (thickness)
				{
					case THICKNESS::THIN:
						setChars(symbol(U'\x2500'), symbol(U'\x2502'), symbol(U'\x250C'), symbol(U'\x2510'), symbol(U'\x2514'), symbol(U'\x2518'));
						break;
					case THICKNESS::MEDIUM:
						setChars(symbol(U'\x2550'), symbol(U'\x2551'), symbol(U'\x2554'), symbol(U'\x2557'), symbol(U'\x255A'), symbol(U'\x255D'));
						break;
					case THICKNESS::THICK:
						setChars(symbol(U'\x2558'), symbol(U'\x2558'), symbol(U'\x2558'), symbol(U'\x2558'), symbol(U'\x2558'), symbol(U'\u2558'));
						break;
				}
			}
			box(surface_size size, symbol h_line, symbol v_line, symbol top_l, symbol top_r, symbol bottom_l, symbol bottom_r)
			{
				setSize(size);
				setChars(h_line, v_line, top_l, top_r, bottom_l, bottom_r);	
			}

			void setColor(color Color)
			{
				horizontal_line.setColor(Color);
				vertical_line.setColor(Color);
				top_left.setColor(Color);
				top_right.setColor(Color);
				bottom_left.setColor(Color);
				bottom_right.setColor(Color);

				fillChars();
			}
			void setChars(symbol h_line, symbol v_line, symbol t_l, symbol t_r, symbol b_l, symbol b_r)
			{
				horizontal_line = h_line;
				vertical_line = v_line;
				top_left = t_l;
				top_right = t_r;
				bottom_left = b_l;
				bottom_right = b_r;

				fillChars();
			}
			void resize_action() { fillChars(); }	
	};



	template<int direction>
	struct scroll : surface
	{
		private:
			symbol m_slider; //handle
			symbol m_line;
			int m_content_length = 0;
			int m_visible_content_length = -1;
			int m_handle_position = 0;
			int m_handle_length = 0;

			int getSurfaceSize() {
				switch (direction)
				{
				case DIRECTION::HORIZONTAL:
					return getSize().x;
					break; 
				case DIRECTION::VERTICAL:
					return getSize().y;
					break;
				}
			}

			int visibleContentLength()
			{
				if (m_visible_content_length < 0) { return getSurfaceSize(); }
				else { return m_visible_content_length; }
			}

			void fill()
			{
				if (isNeeded())
				{
					switch (direction)
					{
					case DIRECTION::HORIZONTAL:
						for (int i = 0; i < getSurfaceSize(); i++) { setSymbolAt(m_line, vec2i(i, 0)); }
						break;
					case DIRECTION::VERTICAL:
						for (int i = 0; i < getSurfaceSize(); i++) { setSymbolAt(m_line, vec2i(0, i)); }
						break;
					}
	
					m_handle_length = ((getSurfaceSize() * 1.f) / m_content_length)* getSurfaceSize();
					if (m_handle_length < 1) { m_handle_length = 1; }			

					float handle_pos_perc = m_handle_position * 1.f / (m_content_length*1.f - visibleContentLength());

					int handle_position = round(getSurfaceSize() * (handle_pos_perc)-m_handle_length * (handle_pos_perc));

					if (m_content_length > visibleContentLength()) {
						switch (direction)
						{
						case DIRECTION::HORIZONTAL:
							for (int i = 0; i < m_handle_length; i++) { setSymbolAt(m_slider, vec2i(i + handle_position, 0)); }
							break;
						case DIRECTION::VERTICAL:
							for (int i = 0; i < m_handle_length; i++) { setSymbolAt(m_slider, vec2i(0, i + handle_position)); }
							break;
						}
					}
				}
				else
				{
					makeTransparent();
				}
			}
		public:
			scroll(int fixed_length, int percentage_length)
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

				setSize(fixed_length, percentage_length);
			}

			void setSize(int fixed_length, int percentage_length)
			{
				surface_size size;

				switch (direction)
				{
				case DIRECTION::HORIZONTAL:
					size = { {fixed_length, 1}, {percentage_length, 1} };
					break;
				case DIRECTION::VERTICAL:
					size = { {1, fixed_length}, {1, percentage_length} };
					break;
				}

				surface::setSize(size);
			}


			void setChars(symbol slider, symbol line)
			{
				m_slider = slider;
				m_line = line;
				fill();
			}
			void setColors(color slider, color line)
			{
				m_slider.setColor(slider);
				m_line.setColor(line);
			}

			bool isNeeded()
			{
				if (m_content_length <= visibleContentLength()) { return false; }
				else { return true; }
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

			void draw_action() { fill(); }
			void resize_action() 
			{
				adjustHandlePositionRespectLength();
				fill();
			}


	};

	struct basic_text : surface
	{
		private:
			console_string m_text;

			void fill()
			{
				makeTransparent();

				for (int i = 0; i < getSize().y; i++)	
				{
					for (int j = 0; j < getSize().x; j++)
					{
						if (i*getSize().x + j >= m_text.size()) { break; } 

						setSymbolAt(m_text[i*getSize().x + j], vec2i(j, i));
					}
				}
			}
		public:
			basic_text(surface_size size, console_string txt)
			{
				setSize(size);
				setText(txt);
			}
			void setText(console_string txt)
			{
				m_text = txt;
				fill();
			}
			console_string getText() { return m_text; }

			//void draw_action() { fill(); }

			void resize_action() { fill(); }
	};

	struct text : surface, active_element
	{
	private:
		basic_text m_text;
		scroll<DIRECTION::VERTICAL> m_scroll;

		console_string m_unprepared_text;
		console_string m_prepared_text;

		bool m_use_prepared_text = true;
		bool m_use_dense_punctuation = false;


		void fill()
		{
			console_string final_string;
			for (int i = 0; i < m_text.getSize().x * m_text.getSize().y; i++)
			{
				if (m_text.getSize().x * m_scroll.getHandlePosition() + i < m_prepared_text.size())
				{
					final_string.push_back(m_prepared_text[m_text.getSize().x * m_scroll.getHandlePosition() + i]);
				}
			}
			m_text.setText(final_string);

			makeTransparent();
			insertSurface(m_text);
			if (m_scroll.isNeeded()) { insertSurface(m_scroll); }
		}
		void prepareText()
		{
			console_string prepared;
			int pos = 0;

			if (getSize().x > 2 && getSize().y >= 1 && m_use_prepared_text)
			{
				while (pos < m_unprepared_text.size())
				{
					for (int j = 0; j < m_text.getSize().x; j++)
					{		
						if (pos >= m_unprepared_text.size()) { break; }

						//omit control code unless equal to '\n' 
						if (m_unprepared_text[pos].getSymbol() < 32 && m_unprepared_text[pos].getSymbol() != '\n')
						{
							pos++;
							continue;
						}
						else if (m_unprepared_text[pos].getSymbol() == '\n')
						{
							pos++;

							for (int i = 0; i < (m_text.getSize().x - j); i++)
							{
								prepared += " ";
							}

							break;
						}

						if (j == m_text.getSize().x - 1
							&& pos +1 < m_unprepared_text.size()
							&& (m_use_dense_punctuation ? !IsPunctuation(m_unprepared_text[pos]) : true)
							&& m_unprepared_text[pos].getSymbol() != (' ')	
							&& (m_use_dense_punctuation ? !IsPunctuation(m_unprepared_text[pos + 1]) : true)
							&& m_unprepared_text[pos + 1].getSymbol() != (' ')
							&& m_unprepared_text[pos + 1].getSymbol() >= 32
							)
						{
							if (m_unprepared_text[pos - 1].getSymbol() != ' ') 
							{
								prepared << m_unprepared_text[pos - 1].getColor();
								prepared << "-";
							}
							else 
							{ 
								prepared << m_unprepared_text[pos - 1].getColor();
								prepared << " "; 
							}
						}
						else
						{
							if (j == 0 && m_unprepared_text[pos].getSymbol() == ' ' && pos !=0) { pos++; }//omit space at start of line
							if (pos < m_unprepared_text.size())
							{
								prepared += m_unprepared_text[pos++];
							}
						}
					}
				}
			}
			else
			{
				prepared = m_unprepared_text;
			}

			m_prepared_text = prepared;
		}
	public:
		int keyUp = KEYBOARD::KEY::UP;
		int keyDown = KEYBOARD::KEY::DOWN;
		int keyPageUp = KEYBOARD::KEY::PGUP;
		int keyPageDown = KEYBOARD::KEY::PGDN;

		text(surface_size size, console_string txt)
			: m_scroll(0, 100)
			, m_text({{-1,0}, {100,100}}, u8"")
			{
				setSize(size);

				m_scroll.setPosition(position({ 0,0 }, { 0,0 }, { POSITION::HORIZONTAL::RIGHT, POSITION::VERTICAL::TOP }));
				setText(txt);		
			}

			void adjustSizes()
			{
				m_text.setSize({ {0,0}, {100,100} });
				updateSurfaceSize(m_text);
				prepareText();
				if (getNumberOfLines() > m_text.getSize().y)
				{
					m_text.setSize({ {-1,0}, {100,100} });
					updateSurfaceSize(m_text);
					prepareText();	
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
				m_scroll.setColors({COLOR::WHITE}, {COLOR::WHITE});
				if (m_scroll.isNeeded()) { insertSurface(m_scroll); }
			}
			void disactivation_action() 
			{
				m_scroll.setColors({COLOR::DARKGRAY}, {COLOR::DARKGRAY});
				if (m_scroll.isNeeded()) { insertSurface(m_scroll); }
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

