#pragma once

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
#include "tui_surface.h"

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
	protected:
		virtual void activationAction() {}
		virtual void disactivationAction() {}
	public:
		void activate() 
		{ 
			active = true; 
			activationAction();
		}
		void disactivate() 
		{ 
			active = false; 
			disactivationAction();
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

	

	struct group : surface
	{
		std::vector<surface*> m_surfaces;


		group(surface_size size)
		{
			setSizeInfo(size);
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

		void resize(vec2i size) { m_buffer.setSizeInfo(size); }

		void updateLastBuffer()
		{
			if (m_last_buffer.getSize() != m_buffer.getSize())
			{
				m_last_buffer.setSizeInfo(m_buffer.getSize());
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

					if (m_buffer.getSymbolAt(vec2i(j, i)).getFirstChar() < pow(2, (sizeof(wchar_t) * 8)) && !IsControl(m_buffer.getSymbolAt(vec2i(j, i)).getFirstChar()))
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