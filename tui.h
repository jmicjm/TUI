#pragma once

#include "tui_console.h"
//#include "tui_input.h"
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



	template<int direction>
	struct scroll : surface
	{
		private:
			console_char m_slider = 186;
			console_char m_line = 179;
			int m_lenght = 0;
			int m_handle_position = 0;
			int m_handle_lenght = 0;

			int getSurfaceSize() {
				switch (direction)
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
					switch (direction)
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

					switch (direction)
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
			scroll(vec2i size, int size_type)
			{
				setSize(size, size_type);
				switch (direction)
				{
				case tui::SCROLL::DIRECTION::HORIZONTAL:
					m_slider = 205;
					m_line = 196;
					break;
				case tui::SCROLL::DIRECTION::VERTICAL:
					m_slider = 186;
					m_line = 179;
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

	struct basic_text : surface
	{
		private:
			std::string m_text;

			void fill()
			{
				makeTransparent();

				for (int i = 0; i < getSize().y; i++)	
				{
					for (int j = 0; j < getSize().x; j++)
					{
						if (i*getSize().x + j >= (int)m_text.size() - 1) { break; } //-1 avoid null termination

						setChar(m_text[i*getSize().x + j], vec2i(j, i));
					}
				}
			}
		public:
			basic_text(vec2i size, int sizeType, std::string txt)
			{
				setSize(size, sizeType);
				setText(txt);
			}
			void setText(std::string txt)
			{
				m_text = txt;
				fill();
			}
			std::string getText() { return m_text; }

			//void draw_action() { fill(); }

			void resize_action() { fill(); }
	};

	struct text : surface
	{
		private:
			basic_text m_text;
			scroll<tui::SCROLL::DIRECTION::VERTICAL> m_scroll;

			std::string m_unprepared_text;
			std::string m_prepared_text;

			void fill()
			{
				std::string final_string;
				for (int i = 0; i < m_text.getSize().x * m_text.getSize().y; i++)
				{
					if (m_text.getSize().x * m_scroll.getHandlePosition() + i <= m_prepared_text.size())
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
				std::string prepared;
				int pos = 0;

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
							if (m_unprepared_text[pos - 1] != ' ') { prepared += "-"; }
							else { prepared += " "; }
						}
						else
						{
							if (j == 0 && m_unprepared_text[pos] == ' ') { pos++; }
							prepared += m_unprepared_text[pos++];
						}
					}
				}
				m_prepared_text = prepared;
			}
		public:
			text(vec2i size, int sizeType, std::string txt) 
			: m_scroll(tui::vec2i(1, 100), tui::SIZE::PERCENTAGE_Y)
			, m_text(vec2i(size.x-1, 100), tui::SIZE::PERCENTAGE_Y, " ")
			{
				setSize(size, sizeType);
				setText(txt);

				m_scroll.setPosition(tui::position(tui::vec2i(0, 0), vec2i(POSITION::HORIZONTAL::RIGHT, POSITION::VERTICAL::TOP)));
			}

			void adjustSizes()
			{
				m_text.resize(vec2i(getSize().x, getSize().y));
				prepareText();
				m_scroll.setLenght(getNumberOfLines());
				if (m_scroll.isNeeded())
				{
					m_text.resize(vec2i(getSize().x - 1, getSize().y));
					prepareText();
					m_scroll.setLenght(getNumberOfLines());
				}
			}

			void setText(std::string txt)
			{
				m_unprepared_text = txt;
				adjustSizes();
				fill();
			}

			std::string getText() { return m_unprepared_text; }
			int getNumberOfLines()
			{
				return ceil(m_prepared_text.size() / (m_text.getSize().x * 1.f));
			}

			void update()
			{
				if (isKeyPressedBuffered(KEYBOARD::KEY::UP)) {
					m_scroll.setHandlePosition(m_scroll.getHandlePosition() - 1);
					fill();
				}
				if (isKeyPressedBuffered(KEYBOARD::KEY::DOWN)) {
					m_scroll.setHandlePosition(m_scroll.getHandlePosition() + 1);
					fill();
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
	};


	struct group : surface
	{
		std::vector<surface*> m_surfaces;


		group(vec2i size, int size_type)
		{
			setSize(size, size_type);
		}


		void addSurface(surface &surf)
		{
			m_surfaces.push_back(&surf);
		}

		void removeSurface(surface &surf)
		{
			for (int i = 0; i < m_surfaces.size(); i++)
			{
				if(m_surfaces[i] = &surf)
				{
					m_surfaces.erase(m_surfaces.begin() + i);
				}
			}
		}

		void draw_action()
		{
			makeTransparent();

			for (int i = 0; i < m_surfaces.size(); i++)
			{
				insertSurface(*m_surfaces[i]);
			}
		}

		void resize_action()
		{

		}
	};

}

