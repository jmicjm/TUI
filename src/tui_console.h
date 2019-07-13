#pragma once

#include "tui_config.h"

#include <vector>

#ifdef TUI_TARGET_SYSTEM_WINDOWS
#define UNICODE
	#include <windows.h>
#endif

#ifdef TUI_TARGET_SYSTEM_LINUX
	#include <sys/ioctl.h>
	#include <unistd.h>
	#include <termios.h>
#endif

#include "tui_utils.h"
#include "tui_text_utils.h"
#include "tui_enums.h"

#include <iostream>
#include <locale>
#include <codecvt>

//#ifdef  TUI_TARGET_SYSTEM_WINDOWS
	#include "tui_input.h"
//#endif



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

		navigation_group() : m_time_limit(std::chrono::milliseconds(300)) {}

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
					//if (KEYBOARD::isKeyPressed(m_key_combo_next[i], TUI_BUFFERED_INPUT))
					{
						act++;
					}
				}
				if (act == m_key_combo_next.size())
				{
					m_was_next_pressed = true;
				}

				if (m_time_limit.isEnd(true))
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
		void activation_action() 
		{
			disactivateAll();
			m_selected = 0;
			if (m_elements.size() > 0)
			{
				m_elements[0]->activate();
			}
		}

	};

	struct surface
	{
		friend class group;

		private:		
			std::vector<std::vector<symbol>> m_symbols;
			position m_position;
			surface_size size_info;

			bool m_resized;

			void resize(vec2i size)
			{
				if (size.x != getSize().x || size.y != getSize().y)
				{
					vec2i new_size = size;
					if (size.x < 1)
					{
						new_size.x = 1;
					}
					if (size.y < 1)
					{
						new_size.y = 1;
					}
					
						m_symbols.resize(new_size.y);
						for (int i = 0; i < m_symbols.size(); i++)
						{
							m_symbols[i].resize(new_size.x);
						}
					
					makeTransparent();
					resize_action();

					m_resized = true;
				}
				else
				{
					m_resized = false;
				}
			}
			
		protected:
			virtual void resize_action() {}
		public:	
			surface() : surface({ {1,1},{0,0} }) {}
			surface(surface_size size)
			{
				setSize(size);
			}

			bool isResized()
			{
				return m_resized;
			}

			struct surface_proxy
			{
				friend class surface;
			private:
				int x;
				std::vector<std::vector<symbol>>* temp;
			public:
				symbol& operator[](int y)
				{
					return (*temp)[y][x];
				}
			};

			surface_proxy operator[](int x)
			{
				surface_proxy proxy;
				proxy.temp = &m_symbols;
				proxy.x = x;

				return proxy;
			}

			virtual void draw_action() {}

			void setSymbolAt(symbol character, vec2i position) { m_symbols[position.y][position.x] = character; }
			symbol getSymbolAt(vec2i position) { return m_symbols[position.y][position.x]; }
			void setPosition(position pos) { m_position = pos; }

			void move(vec2i offset)
			{
				vec2i act_pos = getPosition().getOffset();			
				m_position.setOffset(vec2i(act_pos.x + offset.x, act_pos.y + offset.y));
			}
			
			void setSize(surface_size size)
			{
				size_info = size;
				resize(size.getFixedSize());
			}

			void updateSurfaceSize(surface &obj)
			{
				vec2i perc_size = obj.getSizeInfo().getPercentageSize();
				vec2i int_size = obj.getSizeInfo().getFixedSize();

				int x = (perc_size.x / 100.f) * getSize().x + int_size.x;
				int y = (perc_size.y / 100.f) * getSize().y + int_size.y;

				obj.resize({x,y});
			}

			void insertSurface(surface &obj)
			{
				updateSurfaceSize(obj);
				
				obj.draw_action();


				int x_origin = getSize().x * (obj.getPosition().getRelativePoint().x / 100.f) - obj.getSize().x * (obj.getPosition().getRelativePoint().x / 100.f);
				x_origin += obj.getPosition().getOffset().x;
				x_origin += obj.getPosition().getPercentageOffset().x * getSize().x/ 100.f;

				int y_origin = getSize().y * (obj.getPosition().getRelativePoint().y / 100.f) - obj.getSize().y * (obj.getPosition().getRelativePoint().y / 100.f);
				y_origin += obj.getPosition().getOffset().y;
				y_origin += obj.getPosition().getPercentageOffset().y * getSize().y /100.f;

				for (int y = 0; y < obj.getSize().y; y++)
				{
					for (int x = 0; x < obj.getSize().x; x++)
					{
						if (x_origin + x < getSize().x
							&& y_origin + y < getSize().y
							&& x_origin + x >= 0
							&& y_origin + y >= 0
							&& obj[x][y].getFirstChar() != 0)
						{
							setSymbolAt(obj.getSymbolAt({ x, y }), { x_origin + x, y_origin + y });
						}
					}
				}
			}

			void fill(symbol Symbol)
			{
				for (int y = 0; y < getSize().y; y++)
				{
					for (int x = 0; x < getSize().x; x++)
					{
						setSymbolAt(Symbol, { x,y });
					}
				}
			}

			void makeTransparent() { fill((char32_t)0); }
			void makeBlank() { fill(BLANKSYMBOL); }

			position getPosition() { return m_position; }
			vec2i getSize() 
			{
				if (m_symbols.size() > 0)
				{
					return vec2i(m_symbols[0].size(), m_symbols.size());
				}
				else
				{
					return vec2i(0, 0);
				}
			}
			surface_size getSizeInfo() { return size_info; }
	
	};

	template<int direction>
	struct surface1D : surface
	{
		void setSize(surface1D_size size)
		{
			switch (direction)
			{
			case tui::DIRECTION::HORIZONTAL:
				surface::setSize({ {size.fixed_size, 1},{size.percentage_size, 0} });
				break;
			case tui::DIRECTION::VERTICAL:
				surface::setSize({ {1, size.fixed_size},{0, size.percentage_size} });
				break;
			}
		}
		int getSize() 
		{
				switch (direction)
				{
				case tui::DIRECTION::HORIZONTAL:
					return surface::getSize().x;
				case tui::DIRECTION::VERTICAL:
					return surface::getSize().y;
				}
		}
		void setSymbolAt(symbol character, int position)
		{
			switch (direction)
			{
			case tui::DIRECTION::HORIZONTAL:
				surface::setSymbolAt(character, { position, 0 });
				break;
			case tui::DIRECTION::VERTICAL:
				surface::setSymbolAt(character, { 0, position });
				break;
			}
		}
		symbol getSymbolAt(int position)
		{
			switch (direction)
			{
			case tui::DIRECTION::HORIZONTAL:
				return surface::getSymbolAt({ position, 0 });
			case tui::DIRECTION::VERTICAL:
				return surface::getSymbolAt({ 0, position });
			}
		}
		symbol& operator[](int i)
		{
			switch (direction)
			{
			case tui::DIRECTION::HORIZONTAL:
				return surface::operator[](i)[0];
			case tui::DIRECTION::VERTICAL:
				return surface::operator[](0)[i];
			}
		}
		surface1D_size getSizeInfo()
		{
			switch (direction)
			{
			case tui::DIRECTION::HORIZONTAL:
				return surface1D_size(surface::getSizeInfo().getFixedSize().x, surface::getSizeInfo().getPercentageSize().x);
			case tui::DIRECTION::VERTICAL:
				return surface1D_size(surface::getSizeInfo().getFixedSize().y, surface::getSizeInfo().getPercentageSize().y);;
			}
		}
	};

	struct group : surface
	{
		std::vector<surface*> m_surfaces;


		group(surface_size size)
		{
			setSize(size);
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

		surface m_last_buffer;

		console_buffer() {}

		void resize(vec2i size) { m_buffer.setSize(size); }

		void updateLastBuffer()
		{
			if (m_last_buffer.getSize() != m_buffer.getSize())
			{
				m_last_buffer.setSize(m_buffer.getSize());
			}

			for (int y = 0; y < m_buffer.getSize().y; y++)
			{
				for (int x = 0; x < m_buffer.getSize().x; x++)
				{
					m_last_buffer[x][y] = m_buffer[x][y];
				}
			}
		}
		bool changed()
		{
			if (m_last_buffer.getSize() != m_buffer.getSize())
			{
				return true;
			}

			for (int y = 0; y < m_buffer.getSize().y; y++)
			{
				for (int x = 0; x < m_buffer.getSize().x; x++)
				{
					if (m_buffer[x][y] != m_last_buffer[x][y])
					{
						return true;
					}
				}
			}
			return false;
		}

	public:
		vec2i getSize() { return m_buffer.getSize(); }

		symbol getSymbolAt(vec2i position) { return m_buffer.getSymbolAt(position); }

		void draw(surface &surf) { m_buffer.insertSurface(surf); }

		void updateSurfaceSize(surface &surf) {	m_buffer.updateSurfaceSize(surf); }

		void clear_buf() { m_buffer.makeBlank(); }
	};


	//console
	struct console : console_buffer
	{
	private:
#ifdef TUI_TARGET_SYSTEM_WINDOWS
		HANDLE m_console_handle;
#endif
		vec2i m_last_size;
		bool m_resized;
		time_frame m_fps_control;

	public:
		console() : m_fps_control(std::chrono::milliseconds(1000) / 30)
		{
#ifdef  TUI_TARGET_SYSTEM_WINDOWS
			system("chcp 65001");
			m_console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
#endif

#ifdef TUI_TARGET_SYSTEM_LINUX
			
#endif
			updateSize();
			hidePrompt();
		}


		bool isResized() { return m_resized; }

		void setFPSlimit(int fps)
		{
			m_fps_control.setFrameTime(std::chrono::milliseconds(1000) / fps);
		}

		void clear()
		{
			updateSize();
			clear_buf();
		}

		

		bool isTimeToDisplay()
		{
			return m_fps_control.isEnd(false);
		}

		void display()
		{
			m_fps_control.sleepUntilEnd();

			KEYBOARD::buffer.clear();

			if (!changed()) { return; }

#ifdef  TUI_TARGET_SYSTEM_WINDOWS
			std::vector<CHAR_INFO> temp;

			for (int i = 0; i < getSize().y; i++)
			{
				for (int j = 0; j < getSize().x; j++)
				{
					CHAR_INFO ch_info;

					if (m_buffer.getSymbolAt(vec2i(j, i)).getFirstChar() < pow(2, (sizeof(wchar_t) * 8)) && !isControl(m_buffer.getSymbolAt(vec2i(j, i)).getFirstChar()))
					{
						ch_info.Char.UnicodeChar = m_buffer.getSymbolAt(vec2i(j, i)).getFirstChar();
					}
					else { ch_info.Char.UnicodeChar = '?'; }

					ch_info.Attributes = m_buffer.getSymbolAt({ j,i }).getColor().getRGBIColor();

					temp.push_back(ch_info);
				}
			}

			SMALL_RECT* srect = new SMALL_RECT;
			*srect = { 0,0,(SHORT)getSize().x, (SHORT)getSize().y };

			SetConsoleCursorPosition(m_console_handle, { 0,0 });//without this output may be misplaced after console resize if "wrap text on resize" was selected in console options

			WriteConsoleOutputW(m_console_handle, temp.data(), { (SHORT)getSize().x, (SHORT)getSize().y }, { 0,0 }, srect);

			delete srect;	

			SetConsoleCursorPosition(m_console_handle, { 0,0 });
#endif
#ifdef TUI_TARGET_SYSTEM_LINUX
			
			std::string str;

			for (int i = 0; i < getSize().y; i++)
			{
				for (int j = 0; j < getSize().x; j++)
				{
					str += m_buffer.getSymbolAt({ j, i }).getColor().getEscapeCode();

					if (m_buffer.getSymbolAt({ j, i }).getFirstChar() >= 32)
					{
						//std::u32string u32s = 
						str += Utf32ToUtf8(m_buffer.getSymbolAt({ j, i }).getSymbol());
					}
					else
					{
						str += "?";
					}
				}	
			}

			/* if not doubled last few characters will be displayed after
			some delay. I dont know why this happen*/
			std::cout << str << "\033[H" << str << "\033[H";
			

#endif
			setGlobalColor(color(COLOR::WHITE, COLOR::BLACK));
			hidePrompt();
			updateLastBuffer();
		}

		void setTitle(std::string title) 
		{
#ifdef  TUI_TARGET_SYSTEM_WINDOWS
			SetConsoleTitleA(title.c_str()); 
#endif
		}

	private:
		void updateSize()
		{
			vec2i console_size;

#ifdef  TUI_TARGET_SYSTEM_WINDOWS
			CONSOLE_SCREEN_BUFFER_INFO buffer_info;
			GetConsoleScreenBufferInfo(m_console_handle, &buffer_info);
			
			console_size.x = buffer_info.srWindow.Right - buffer_info.srWindow.Left + 1;
			console_size.y = buffer_info.srWindow.Bottom - buffer_info.srWindow.Top + 1;
#endif

#ifdef TUI_TARGET_SYSTEM_LINUX
			winsize w;
			ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

			console_size.x = w.ws_col;
			console_size.y = w.ws_row;
#endif

			if (console_size.x != m_last_size.x || console_size.y != m_last_size.y)
			{
				resize(console_size);
				m_resized = true;
			}
			else { m_resized = false; }

			m_last_size = console_size;
		}

		void setGlobalColor(color color) 
		{
#ifdef TUI_TARGET_SYSTEM_WINDOWS
			SetConsoleTextAttribute(m_console_handle, color.getRGBIColor()); 
#endif

#ifdef TUI_TARGET_SYSTEM_LINUX
			std::cout << color.getEscapeCode();
#endif
		}

		void hidePrompt()
		{
#ifdef TUI_TARGET_SYSTEM_WINDOWS
			CONSOLE_CURSOR_INFO cursor_info;
			SetConsoleCursorInfo(m_console_handle, &cursor_info);
			cursor_info.bVisible = false;
			cursor_info.dwSize = 1;

			SetConsoleCursorInfo(m_console_handle, &cursor_info);
#endif

#ifdef TUI_TARGET_SYSTEM_LINUX
			std::cout << "\033[?25l";
#endif
		}
	};
}