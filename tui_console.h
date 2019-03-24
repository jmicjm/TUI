#pragma once
#include <vector>

#include <windows.h>

#include "tui_utils.h"
#include "tui_enums.h"
#include "tui_input.h"



namespace tui
{
	struct active_element
	{
	private:
		bool active = false;
	public:
		virtual void activation_action() {}
		virtual void disactivation_action() {}

		void activate() 
		{ 
			active = true; 
			activation_action();
		}
		void disactivate() 
		{ 
			active = false; 
			disactivation_action();
		}
		bool isActive() { return active; }
	};

	struct navigation_group : active_element
	{
	private:
		std::vector<int> m_key_combo_next;
		std::vector<active_element*> m_elements;

		bool m_was_next_pressed = false;

		int m_selected = -1;

		time_frame m_time_limit;


		void disactivateAll()
		{
			for (int i = 0; i < m_elements.size(); i++)
			{
				m_elements[i]->disactivate();
			}
		}

	public:

		navigation_group() : m_time_limit(std::chrono::milliseconds(1000)) {}

		void setKeyComboNext(std::vector<int> combo)
		{
			m_key_combo_next = combo;
		}
		void addElement(active_element &element)
		{
			m_elements.push_back(&element);
		}

		void update()
		{
			if (isActive())
			{

				int act = 0;

				for (int i = 0; i < m_key_combo_next.size(); i++)
				{
					if (KEYBOARD::isKeyPressed(m_key_combo_next[i], TUI_BUFFERED_INPUT))
					{
						act++;
					}
				}
				if (act == m_key_combo_next.size())
				{
					m_was_next_pressed = true;
				}

				if (m_time_limit.isEnd())
				{
					

					if (m_was_next_pressed)
					{
						disactivateAll();

						if (m_selected < (int)m_elements.size() - 1) { m_selected++; }
						else { m_selected = 0; }
						m_elements[m_selected]->activate();

						m_was_next_pressed = false;
					}
				}
			}
		}

		void disactivation_action() { disactivateAll(); }

	};

	struct surface
	{
		friend class group;

		private:		
			std::vector<std::vector<console_char>> m_chars;
			position m_position;
			vec2i percentageSize;
			int sizeType;
			
		protected:
			virtual void resize_action() {}
		public:	
			surface()
			{
				m_chars.resize(1);
				m_chars[0].resize(1);
			}

			virtual void draw_action() {}

			void setSizeType(int type) { sizeType = type; }

			void setChar(console_char character, vec2i position) { m_chars[position.x][position.y] = character; }
			console_char getchar(vec2i position) { return m_chars[position.x][position.y]; }
			void setPosition(position pos) { m_position = pos; }

			void move(vec2i offset)
			{
				vec2i act_pos = getPosition().getOffset();			
				m_position.setOffset(vec2i(act_pos.x + offset.x, act_pos.y + offset.y));
			}

			void resize(vec2i size)
			{
				if (size.x != getSize().x || size.y != getSize().y)
				{
					m_chars.resize(size.x);
					for (int i = 0; i < m_chars.size(); i++)
					{
						m_chars[i].resize(size.y);
					}
					makeTransparent();
					resize_action();
				}
			}
			
			void setSize(vec2i size, int sizeType)
			{
				setSizeType(sizeType);
				percentageSize = size;

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
				if (m_surfaces[i] == &surf)
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



	//buffer
	struct console_buffer
	{
	protected:
		surface m_buffer;

		console_buffer() { m_buffer.setSizeType(SIZE::CONSTANT); }

		void resize(vec2i size) { m_buffer.resize(size); }

	public:
		vec2i getSize() { return m_buffer.getSize(); }

		console_char getChar(vec2i position) { return m_buffer.getChar(position); }

		void draw(surface &surf) { m_buffer.insertSurface(surf); }

		void clear_buf() { m_buffer.makeTransparent(); }
	};


	//console
	struct console : console_buffer
	{
	private:
		HANDLE console_handle;
		vec2i m_last_size;
		bool resized;

		time_frame fps_control;

	public:
		console() : fps_control(std::chrono::milliseconds(1000) / 30)
		{
			system("chcp 437");
			console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
			//SetConsoleDisplayMode(GetStdHandle(STD_OUTPUT_HANDLE), CONSOLE_FULLSCREEN_MODE, 0);
			//SetConsoleMode(console_handle, ENABLE_WRAP_AT_EOL_OUTPUT);

			updateSize();
			hidePrompt();
		}

		bool wasResized() { return resized; }

		void setFPSlimit(int fps)
		{
			fps_control.setFrameTime(std::chrono::milliseconds(1000) / fps);
		}

		void clear()
		{
			updateSize();
			clear_buf();
		}

		void display()
		{
			fps_control.sleepUntilEnd();

			KEYBOARD::buffer.clear();

			//updateSize();

			CONSOLE_SCREEN_BUFFER_INFO buffer_info;
			GetConsoleScreenBufferInfo(console_handle, &buffer_info);
			vec2i console_size(buffer_info.dwSize.X, buffer_info.dwSize.Y);


			std::vector<WORD> temp_attr;
			std::vector<char> temp_char;

			for (int i = 0; i < getSize().y; i++)
			{
				for (int j = 0; j < console_size.x; j++)
				{
					if (j < getSize().x)
					{
						temp_attr.push_back(m_buffer.getChar(vec2i(j, i)).getColor());
						temp_char.push_back(m_buffer.getChar(vec2i(j, i)).getChar());
					}
					else
					{
						temp_attr.push_back(console_color());
						temp_char.push_back(char());
					}
				}
			}
			COORD coord = { 0, 0 };
			DWORD useless = 0;
			SetConsoleCursorPosition(console_handle, coord); // w/o 10x slower

			WriteConsoleOutputAttribute(console_handle, temp_attr.data(), console_size.x*getSize().y, coord, &useless);
			WriteConsoleOutputCharacter(console_handle, temp_char.data(), console_size.x*getSize().y, coord, &useless);


			setGlobalColor(console_color(COLOR::WHITE, COLOR::BLACK));
			SetConsoleCursorPosition(console_handle, coord);

			hidePrompt();
		}

		void setTitle(std::string title) { SetConsoleTitleA(title.c_str()); }

	private:
		void updateSize()
		{
			CONSOLE_SCREEN_BUFFER_INFO buffer_info;
			GetConsoleScreenBufferInfo(console_handle, &buffer_info);
			vec2i console_size(buffer_info.srWindow.Right - buffer_info.srWindow.Left + 1, buffer_info.srWindow.Bottom - buffer_info.srWindow.Top + 1);
			//vec2i console_size(buffer_info.dwSize.X, buffer_info.dwSize.Y);


			if (console_size.x != m_last_size.x || console_size.y != m_last_size.y)
			{
				resize(console_size);
				//SetConsoleScreenBufferSize(console_handle, { (short)console_size.x, (short)console_size.y });

				resized = true;

				

				
				//HWND x = GetConsoleWindow();
				//ShowScrollBar(x, SB_BOTH, FALSE);
			}
			else
			{
				resized = false;
			}
			//COORD = {}
			

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