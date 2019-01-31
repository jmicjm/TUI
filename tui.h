#pragma once

#include "tui_console.h"
//#include "tui_utils.h"
//#include "tui_enums.h"
#include <string>
#include <iostream>

namespace tui 
{
	struct rectangle : surface
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
			box(vec2i size, int sizeType, int thickness)
			{
				setSize(size, sizeType);
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
			box(vec2i size, int sizeType, console_char h_line, console_char v_line, console_char top_l, console_char top_r, console_char bottom_l, console_char bottom_r)
			{
				setSize(size, sizeType);
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



	template<int type>
	struct scroll : surface
	{
		private:
			console_char m_slider = 186;
			console_char m_line = 179;
			int m_lenght = 0;
			int m_handle_position = 0;
			int m_handle_lenght = 0;

			int getSurfaceSize() {
				switch (type)
				{
				case tui::SCROLL::DIRECTION::HORIZONTAL:
					return getSize().x;
					break; 
				case tui::SCROLL::DIRECTION::VERTICAL:
					return getSize().y;
					break;
				}
			}

			void fill()
			{
				if (isNeeded())
				{
					switch (type)
					{
					case tui::SCROLL::DIRECTION::HORIZONTAL:
						for (int i = 0; i < getSurfaceSize(); i++) { setChar(m_line, vec2i(i, 0)); }
						break;
					case tui::SCROLL::DIRECTION::VERTICAL:
						for (int i = 0; i < getSurfaceSize(); i++) { setChar(m_line, vec2i(0, i)); }
						break;
					}
	
					m_handle_lenght = ((getSurfaceSize() * 1.f) / m_lenght)* getSurfaceSize();
					if (m_handle_lenght < 1) { m_handle_lenght = 1; }

					float handle_pos_perc = m_handle_position * 1.f / (m_lenght*1.f - getSurfaceSize());

					int handle_position = round(getSurfaceSize() * (handle_pos_perc)-m_handle_lenght * (handle_pos_perc));

					switch (type)
					{
					case tui::SCROLL::DIRECTION::HORIZONTAL:
						for (int i = 0; i < m_handle_lenght; i++) { setChar(m_slider, vec2i(i + handle_position, 0)); }		
						break;
					case tui::SCROLL::DIRECTION::VERTICAL:
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
			scroll(int lenght, int lenght_type)
			{
				switch (type)
				{
				case tui::SCROLL::DIRECTION::HORIZONTAL:
					if(lenght_type == tui::SCROLL::LENGHT::CONSTANT) { setSize(vec2i(lenght, 1), tui::SIZE::CONSTANT); }
					else{ setSize(vec2i(lenght, 1), tui::SIZE::PERCENTAGE_X); }
					break;
				case tui::SCROLL::DIRECTION::VERTICAL:
					if (lenght_type == tui::SCROLL::LENGHT::CONSTANT) { setSize(vec2i(1, lenght), tui::SIZE::CONSTANT); }
					else{ setSize(vec2i(1, lenght), tui::SIZE::PERCENTAGE_Y); }
					break;
				}
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

			void draw_action() { update(); }
			void resize_action() 
			{
				adjustHandlePositionRespectLenght();
				fill();
			}
	};


	bool isPunctuation(char symbol)
	{
		if(    symbol == '.'
			|| symbol == ','
			|| symbol == ':'
			|| symbol == ';'
			|| symbol == '!'
			|| symbol == '?')
			 { return true; }
		else { return false; }
	}

	struct text : surface
	{
		private:
			std::string m_text;
			//std::string m_prepared_text;

			void fill()
			{
				std::string p_text = getPreparedText(getSize().x);
				int pos = 0;
				
				for (int i = 0; i < getSize().y; i++)
				{
					for (int j = 0; j < getSize().x; j++)
					{
						if (i*getSize().x + j > p_text.size()) { break; }
								
						setChar(p_text[pos++], vec2i(j, i));				
					}
				}		
			}
			std::string getPreparedText(int width)
			{
				std::string prepared;
				int pos = 0;

				while (pos < m_text.size())
				{
					for (int j = 0; j < getSize().x; j++)
					{
						if (pos >= m_text.size()) { break; }
						if (j == getSize().x - 1
							&& !isPunctuation(m_text[pos])
							&& m_text[pos] != (' ')
							&& m_text[pos + 1] != (' ')
							&& !isPunctuation(m_text[pos + 1]))
						{
							if (m_text[pos - 1] != ' ') 
							{ 
								prepared += "-";
								pos++;
							}
							else 
							{ 
								prepared += " ";
								pos++;
							}
						}

						else
						{
							if (j == 0 && m_text[pos] == ' ') { pos++; }
							prepared += m_text[pos++];
						}
					}
				}

				return prepared;
			}
		public:
			text(vec2i size, int sizeType, std::string txt)
			{
				setSize(size, sizeType);
				setText(txt);
			}
			void setText(std::string txt)
			{
				m_text = txt;	
				fill();
			}
			std::string getText()
			{
				return m_text;
			}

			void resize_action() { fill(); }
	};

}

