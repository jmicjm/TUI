#pragma once

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
			console_char m_character;

			void fill()
			{

				for (int i = 0; i < getSize().x; i++)
				{
					for (int j = 0; j < getSize().y; j++) {
						surface::setChar(m_character, vec2i(i, j));
					}
				}
			}
		public:
			void setChar(console_char character)
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
			console_char horizontal_line;
			console_char vertical_line;
			console_char top_left;
			console_char top_right;
			console_char bottom_left;
			console_char bottom_right;

			void fillChars()
			{
				surface::setChar(top_left, vec2i(0, 0));
				surface::setChar(top_right, vec2i(getSize().x - 1, 0));
				surface::setChar(bottom_left, vec2i(0, getSize().y - 1));
				surface::setChar(bottom_right, vec2i(getSize().x - 1, getSize().y - 1));

				for(int i = 1; i < getSize().x-1; i++) { surface::setChar(horizontal_line, vec2i(i, 0)); }
				for(int i = 1; i < getSize().x-1; i++) { surface::setChar(horizontal_line, vec2i(i, getSize().y - 1)); }

				for (int i = 1; i < getSize().y-1; i++) { surface::setChar(vertical_line, vec2i(0, i)); }
				for (int i = 1; i < getSize().y-1; i++) { surface::setChar(vertical_line, vec2i(getSize().x - 1, i)); }
			}
		public:
			box(surface_size size, int thickness)
			{
				setSize(size);
				switch (thickness)
				{
					case THICKNESS::THIN:
						setChars(console_char(196), console_char(179), console_char(218), console_char(191), console_char(192), console_char(217));
						break;
					case THICKNESS::MEDIUM:
						setChars(console_char(205), console_char(186), console_char(201), console_char(187), console_char(200), console_char(188));
						break;
					case THICKNESS::THICK:
						setChars(console_char(219), console_char(219), console_char(219), console_char(219), console_char(219), console_char(219));
						break;
				}
			}
			box(surface_size size, console_char h_line, console_char v_line, console_char top_l, console_char top_r, console_char bottom_l, console_char bottom_r)
			{
				setSize(size);
				setChars(h_line, v_line, top_l, top_r, bottom_l, bottom_r);	
			}

			void setColor(console_color color)
			{
				horizontal_line.setColor(color);
				vertical_line.setColor(color);
				top_left.setColor(color);
				top_right.setColor(color);
				bottom_left.setColor(color);
				bottom_right.setColor(color);

				fillChars();
			}
			void setChars(console_char h_line, console_char v_line, console_char t_l, console_char t_r, console_char b_l, console_char b_r)
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
			console_char m_slider = 186; //handle
			console_char m_line = 179;
			int m_lenght = 0;
			int m_handle_position = 0;
			int m_handle_lenght = 0;

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

			void fill()
			{
				if (isNeeded())
				{
					switch (direction)
					{
					case DIRECTION::HORIZONTAL:
						for (int i = 0; i < getSurfaceSize(); i++) { setChar(m_line, vec2i(i, 0)); }
						break;
					case DIRECTION::VERTICAL:
						for (int i = 0; i < getSurfaceSize(); i++) { setChar(m_line, vec2i(0, i)); }
						break;
					}
	
					m_handle_lenght = ((getSurfaceSize() * 1.f) / m_lenght)* getSurfaceSize();
					if (m_handle_lenght < 1) { m_handle_lenght = 1; }

					float handle_pos_perc = m_handle_position * 1.f / (m_lenght*1.f - getSurfaceSize());

					int handle_position = round(getSurfaceSize() * (handle_pos_perc)-m_handle_lenght * (handle_pos_perc));

					switch (direction)
					{
					case DIRECTION::HORIZONTAL:
						for (int i = 0; i < m_handle_lenght; i++) { setChar(m_slider, vec2i(i + handle_position, 0)); }		
						break;
					case DIRECTION::VERTICAL:
						for (int i = 0; i < m_handle_lenght; i++) { setChar(m_slider, vec2i(0, i + handle_position)); }
						break;
					}				
				}
				else
				{
					makeTransparent();
				}
			}
		public:
			scroll(surface_size size)
			{
				switch (direction)
				{
				case DIRECTION::HORIZONTAL:
					m_slider = 205;
					m_line = 196;
					break;
				case DIRECTION::VERTICAL:
					m_slider = 186;
					m_line = 179;
					break;
				}

				setSize(size);
			}

			void setChars(console_char slider, console_char line)
			{
				m_slider = slider;
				m_line = line;
				fill();
			}
			void setColors(console_color slider, console_color line)
			{
				m_slider.setColor(slider);
				m_line.setColor(line);
			}

			bool isNeeded()
			{
				if (m_lenght <= getSurfaceSize()) { return false; }
				else { return true; }
			};

			void adjustHandlePositionRespectLenght()
			{
				if (!isNeeded())
				{
					m_handle_position = 0;
				}
				else if (m_handle_position > m_lenght - getSurfaceSize())
				{
					m_handle_position = m_lenght - getSurfaceSize();
				}
			}

			void adjustHandlePositionRespectBounds()
			{
				adjustHandlePositionRespectLenght();

				if (m_handle_position < 0) { m_handle_position = 0; }
			}

			void setLenght(int lenght) 
			{
				if (lenght >= 0) { m_lenght = lenght; }
				else { m_lenght = 0; }

				adjustHandlePositionRespectLenght();
				fill();
			}

			void setHandlePosition(int handle_position) 
			{
				m_handle_position = handle_position;

				adjustHandlePositionRespectBounds();	
				fill();
			}

			int getLenght() { return m_lenght; }
			int getHandlePosition() { return m_handle_position; }

			
			void update() 
			{
				//fill(); 
			}

			void draw_action() { fill(); }
			void resize_action() 
			{
				adjustHandlePositionRespectLenght();
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

						setChar(m_text[i*getSize().x + j], vec2i(j, i));
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

		int keyUp = KEYBOARD::KEY::UP;
		int keyDown = KEYBOARD::KEY::DOWN;

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

			if (getSize().x > 2 && getSize().y >= 1)
			{
				while (pos < m_unprepared_text.size())
				{
					for (int j = 0; j < m_text.getSize().x; j++)
					{
						if (pos >= m_unprepared_text.size()) { break; }
						if (j == m_text.getSize().x - 1
							&& !isPunctuation(m_unprepared_text[pos])
							&& m_unprepared_text[pos] != (' ')
							&& m_unprepared_text[pos + 1] != (' ')
							&& !isPunctuation(m_unprepared_text[pos + 1]))
						{
							if (m_unprepared_text[pos - 1] != ' ') {
								prepared << m_unprepared_text[pos - 1].getColor();
								prepared << "-";
							}
							else { prepared += ' '; }
						}
						else
						{
							if (j == 0 && m_unprepared_text[pos] == ' ') { pos++; }
							prepared += m_unprepared_text[pos++];
						}
					}
				}
			}
			else
			{
				prepared = m_unprepared_text;
			}
			//prepared += ' ';
			m_prepared_text = prepared;
		}
	public:
		text(surface_size size, console_string txt)
			: m_scroll({{1,0},{0,100}})
			, m_text({{-1,0}, {100,100}}, " ")
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
				m_scroll.setLenght(getNumberOfLines());
				if (m_scroll.isNeeded())
				{
					m_text.setSize({ {-1,0}, {100,100} });
					updateSurfaceSize(m_text);
					prepareText();
					m_scroll.setLenght(getNumberOfLines());
				}
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

			void setUpKey(int key) { keyUp = key; }
			int getUpKey() { return keyUp; }
			void setDownKey(int key) { keyDown = key; }
			int getDownKey() { return keyDown; }

			void update()
			{
				if (isActive()) {
					if (KEYBOARD::isKeyPressedBuffered(keyUp)) {
						lineUp();
					}
					if (KEYBOARD::isKeyPressedBuffered(keyDown)) {
						lineDown();
					}
				}
			}


			void draw_action() 
			{
				update();
			}
			void resize_action()
			{
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

		console_char m_bar;
		console_char m_start;
		console_char m_end;

		void fill()
		{
			
		}

	};


	

}

