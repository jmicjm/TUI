#pragma once
#include <vector>
#include <windows.h>

#include "tui_utils.h"
#include "tui_enums.h"



namespace tui
{
	struct surface
	{
		private:		
			std::vector<std::vector<console_char>> m_chars;
			position m_position;
			vec2i percentageSize;
			int sizeType;
			void setSizeType(int type) { sizeType = type; }
		protected:
			virtual void resize_action() {}
		public:	
			virtual void draw_action() {}

			void setChar(console_char character, vec2i position) { m_chars[position.x][position.y] = character; }
			void setPosition(position pos) { m_position = pos; }

			void move(vec2i offset)
			{
				vec2i act_pos = getPosition().getOffset();			
				m_position.setOffset(vec2i(act_pos.x + offset.x, act_pos.y + offset.y));
			}

			void resize(vec2i size)
			{
				m_chars.resize(size.x);
				for (int i = 0; i < m_chars.size(); i++)
				{
					m_chars[i].resize(size.y);
				}
				makeTransparent();
				resize_action();
			}
			
			void setSize(vec2i size, int sizeType)
			{
				setSizeType(sizeType);
				percentageSize = size;
			/*	switch (getSizeType())
				{
					case SIZE::CONSTANT:
						setSizeType(SIZE::CONSTANT);
						break;
					case SIZE::PERCENTAGE:
						setSizeType(SIZE::PERCENTAGE);
						percentageSize = size;
						break;
					case SIZE::PERCENTAGE_X:
						setSizeType(SIZE::PERCENTAGE_X);
						percentageSize = size;
						break;
					case SIZE::PERCENTAGE_Y:
						setSizeType(SIZE::PERCENTAGE_Y);
						percentageSize = size;
						break;
				}*/
				resize(size);
			}

			void updateSurfaceSize(surface &obj)
			{
				vec2i perc_size = obj.getPercentageSize();
				switch (obj.getSizeType())
				{
				case SIZE::CONSTANT:
					//	obj.resize(obj.getSize());
					break;
				case SIZE::PERCENTAGE:
					obj.resize(vec2i((perc_size.x / 100.f) * getSize().x, (perc_size.y / 100.f) * getSize().y));
					break;
				case SIZE::PERCENTAGE_X:
					obj.resize(vec2i((perc_size.x / 100.f) * getSize().x, obj.getSize().y));
					break;
				case SIZE::PERCENTAGE_Y:
					obj.resize(vec2i(obj.getSize().x, (perc_size.y / 100.f) * getSize().y));
					break;
				}
			}

			void insertSurface(surface &obj)
			{
				updateSurfaceSize(obj);
				
				obj.draw_action();


				int x_origin = getSize().x * (obj.getPosition().getRelativePoint().x / 100.f) - obj.getSize().x * (obj.getPosition().getRelativePoint().x / 100.f);
				x_origin += obj.getPosition().getOffset().x;

				int y_origin = getSize().y * (obj.getPosition().getRelativePoint().y / 100.f) - obj.getSize().y * (obj.getPosition().getRelativePoint().y / 100.f);
				y_origin += obj.getPosition().getOffset().y;

				for (int i = 0; i < obj.getSize().x; i++)
				{
					for (int j = 0; j < obj.getSize().y; j++)
					{
						if (x_origin + i < getSize().x
							&& y_origin + j < getSize().y
							&& x_origin + i >= 0
							&& y_origin + j >= 0
							&& obj.getChar(vec2i(i, j)).getChar() != TRANSPARENT)
						{
							m_chars[x_origin + i][y_origin + j] = obj.getChar(vec2i(i, j));
						}
					}
				}
			}


			void makeTransparent()
			{
				for (int i = 0; i < getSize().x; i++)
				{
					for (int j = 0; j < getSize().y; j++)
					{
						m_chars[i][j] = TRANSPARENT;
					}
				}
			}
			void makeBlank()
			{
				for (int i = 0; i < getSize().x; i++)
				{
					for (int j = 0; j < getSize().y; j++)
					{
						m_chars[i][j] = BLANKSYMBOL;
					}
				}
			}

			int getSizeType() { return sizeType; }
			vec2i getPercentageSize() { return percentageSize; }
			position getPosition() { return m_position; }
			vec2i getSize() { return vec2i(m_chars.size(), m_chars[0].size()); }
			console_char getChar(vec2i position) { return m_chars[position.x][position.y]; }
			
			operator surface() { return *this; }
	};




	//buffer
	struct console_buffer
	{
		protected:
			std::vector<std::vector<console_char>> m_buffer;

			console_buffer() {}
			console_buffer(int x, int y) { resize(x, y); }
			console_buffer(vec2i size) : console_buffer(size.x, size.y) {}

			void resize(int x, int y)
			{
				m_buffer.resize(x);
				for (int i = 0; i < m_buffer.size(); i++) { m_buffer[i].resize(y); }
			}
			void resize(vec2i size) { resize(size.x, size.y); }
		public:
			vec2i getSize() { return vec2i(m_buffer.size(), m_buffer[0].size()); }
	};


	//console
	struct console : console_buffer
	{
		//private:
			HANDLE console_handle;
			vec2i m_last_size;
			bool resized;
		public:
			console()
			{
				system("chcp 437");
				console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
				updateSize();
				hidePrompt();
			}

			bool wasResized()
			{
				return resized;
			}

			void draw(surface &obj)
			{
				if (wasResized())
				{
					vec2i perc_size = obj.getPercentageSize();
					switch (obj.getSizeType())
					{
					case SIZE::CONSTANT:
					//	obj.resize(obj.getSize());
						break;
					case SIZE::PERCENTAGE:
						obj.resize(vec2i((perc_size.x / 100.f) * getSize().x, (perc_size.y / 100.f) * getSize().y));
						break;
					case SIZE::PERCENTAGE_X:
						obj.resize(vec2i((perc_size.x / 100.f) * getSize().x, obj.getSize().y));
						break;
					case SIZE::PERCENTAGE_Y:
						obj.resize(vec2i(obj.getSize().x, (perc_size.y / 100.f) * getSize().y));
						break;
					}
				}
				
				obj.draw_action();
				

				int x_origin = getSize().x * (obj.getPosition().getRelativePoint().x / 100.f) - obj.getSize().x * (obj.getPosition().getRelativePoint().x / 100.f);
				x_origin += obj.getPosition().getOffset().x;

				int y_origin = getSize().y * (obj.getPosition().getRelativePoint().y / 100.f) - obj.getSize().y * (obj.getPosition().getRelativePoint().y / 100.f);
				y_origin += obj.getPosition().getOffset().y;

				for (int i = 0; i < obj.getSize().x; i++)
				{
					for (int j = 0; j < obj.getSize().y; j++)
					{
						if (x_origin + i < getSize().x
							&& y_origin + j < getSize().y
							&& x_origin + i >= 0
							&& y_origin + j >= 0
							&& obj.getChar(vec2i(i, j)).getChar() != TRANSPARENT)
						{
							m_buffer[x_origin + i][y_origin + j] = obj.getChar(vec2i(i, j));
						}
					}
				}
			}
			void clear()
			{
				//updateSize();
				for (int i = 0; i < getSize().x; i++)
				{
					for (int j = 0; j < getSize().y; j++)
					{
						m_buffer[i][j].setChar(' ');
						m_buffer[i][j].setColor(console_color(COLOR::WHITE, COLOR::BLACK));
					}
				}
			}

			void display()
			{
				updateSize();

				std::vector<WORD> temp_attr;
				std::vector<char> temp_char;

				for (int i = 0; i < getSize().y; i++)
				{
					for (int j = 0; j < getSize().x; j++)
					{
						temp_attr.push_back(m_buffer[j][i].getColor());
						temp_char.push_back(m_buffer[j][i].getChar());
					}
				}
				COORD coord = { 0, 0 };
				DWORD useless = 0;
				SetConsoleCursorPosition(console_handle, coord); // w/o 10x slower

				WriteConsoleOutputAttribute(console_handle, temp_attr.data(), getSize().x*getSize().y, coord, &useless);
				WriteConsoleOutputCharacter(console_handle, temp_char.data(), getSize().x*getSize().y, coord, &useless);

				setGlobalColor(console_color(COLOR::WHITE, COLOR::BLACK));
				SetConsoleCursorPosition(console_handle, coord);

				hidePrompt();
			}

			void setChar(char character, vec2i position, console_color color)
			{
				m_buffer[position.x][position.y].setChar(character);
				m_buffer[position.x][position.y].setColor(color);
			}
			void setChar(char character, vec2i position) { setChar(character, position, console_color(COLOR::WHITE, COLOR::BLACK)); }
			void setTitle(std::string title) { SetConsoleTitleA(title.c_str()); }

		private:
			void updateSize()
			{
				CONSOLE_SCREEN_BUFFER_INFO buffer_info;
				GetConsoleScreenBufferInfo(console_handle, &buffer_info);
				vec2i console_size(buffer_info.srWindow.Right - buffer_info.srWindow.Left + 1, buffer_info.srWindow.Bottom - buffer_info.srWindow.Top + 1);

				if (console_size.x != m_last_size.x || console_size.y != m_last_size.y)
				{
					resize(console_size);
					resized = true;
				}
				else
				{
					resized = false;
				}

				m_last_size = console_size;
				
			}
			void setGlobalColor(int color) { SetConsoleTextAttribute(console_handle, color); }
			void hidePrompt()
			{
				CONSOLE_CURSOR_INFO cursor_info;
				SetConsoleCursorInfo(console_handle, &cursor_info);
				cursor_info.bVisible = false;
				cursor_info.dwSize = 1;

				SetConsoleCursorInfo(console_handle, &cursor_info);
			}
		};
}