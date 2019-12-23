#pragma once

#ifdef TUI_TARGET_SYSTEM_WINDOWS
	#include <windows.h>
#endif

#ifdef TUI_TARGET_SYSTEM_LINUX
	#include <sys/ioctl.h>
	#include <unistd.h>
	#include <termios.h>
#endif

#include "tui_utils.h"
#include "tui_text_utils.h"
#include "tui_surface.h"
#include "tui_input.h"

#include <vector>
#include <array>
#include <iostream>



namespace tui
{
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
		bool isChanged()
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
		void clearBuffer() { m_buffer.makeBlank(); }

	public:
		vec2i getSize() { return m_buffer.getSize(); }

		symbol getSymbolAt(vec2i position) { return m_buffer.getSymbolAt(position); }

		void draw(surface &surf) { m_buffer.insertSurface(surf); }

		void updateSurfaceSize(surface &surf) {	m_buffer.updateSurfaceSize(surf); }
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

		bool m_display_rgb = true;
		bool m_display_rgbi = true;
	public:
		console() : m_fps_control(std::chrono::milliseconds(1000) / 30)
		{
#ifdef  TUI_TARGET_SYSTEM_WINDOWS
			system("chcp 65001");
			m_console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
			updateSize();
			hidePrompt();
		}

		bool isResized() { return m_resized; }

		void setFPSlimit(int fps)
		{
			m_fps_control.setFrameTime(std::chrono::milliseconds(1000) / fps);
		}

		void displayRgbColor(bool display) { m_display_rgb = display; }
		bool isDisplayingRgbColor() { return m_display_rgb; }

		void displayRgbiColor(bool display) { m_display_rgbi = display; }
		bool isDisplayingRgbiColor() { return m_display_rgbi; }

		void displayColor(bool display)
		{
			m_display_rgb = display;
			m_display_rgbi = display;
		}
		bool isDisplayingColor() { return m_display_rgbi || m_display_rgb; }

		void clear()
		{
			updateSize();
			clearBuffer();
		}

		bool isTimeToDisplay()
		{
			return m_fps_control.isEnd(false);
		}

		void display()
		{
			m_fps_control.sleepUntilEnd();

			input::swap();

			if (!isChanged()) { return; }

			static const std::array<rgb, 16> colorRgbi = 
			{
				COLOR::BLACK, COLOR::BLUE, COLOR::GREEN, COLOR::CYAN,
				COLOR::RED, COLOR::MAGENTA, COLOR::BROWN, COLOR::LIGHTGRAY,
				COLOR::DARKGRAY, COLOR::LIGHTBLUE, COLOR::LIGHTGREEN, COLOR::LIGHTCYAN,
				COLOR::LIGHTRED, COLOR::LIGHTMAGENTA, COLOR::YELLOW, COLOR::WHITE 
			};

			auto rgbDst = [](rgb c1, rgb c2)
			{
				return (c1.r - c2.r) * (c1.r - c2.r) + (c1.g - c2.g)* (c1.g - c2.g) + (c1.b - c2.b)* (c1.b - c2.b);
			};

			auto rgbToRgbi = [&](rgb c)
			{
				int smallest_dst_id = 0;
				int smallest_dst = rgbDst(c, colorRgbi[0]);

				for (int i = 1; i < colorRgbi.size(); i++)
				{
					if (rgbDst(c, colorRgbi[i]) < smallest_dst)
					{
						smallest_dst = rgbDst(c, colorRgbi[i]);
						smallest_dst_id = i;
					}
				}
				return smallest_dst_id;
			};

#ifdef  TUI_TARGET_SYSTEM_WINDOWS
			std::vector<CHAR_INFO> temp;

			auto getRgbiColor = [&](color c)
			{
				return 16 * rgbToRgbi(c.background) + rgbToRgbi(c.foreground);
			};

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

					if (m_display_rgbi)
					{
						ch_info.Attributes = getRgbiColor(m_buffer.getSymbolAt({ j,i }).getColor());
					}
					else
					{
						ch_info.Attributes = getRgbiColor({ COLOR::WHITE, COLOR::BLACK });
					}

					if (m_buffer.getSymbolAt({ j,i }).isUnderscore())
					{
						ch_info.Attributes |= COMMON_LVB_UNDERSCORE;
					}

					temp.push_back(ch_info);
				}
			}

			SetConsoleCursorPosition(m_console_handle, { 0,0 });//without this output may be misplaced after console resize if "wrap text on resize" was selected in console options

			SMALL_RECT srect = { 0,0,(SHORT)getSize().x, (SHORT)getSize().y };
			WriteConsoleOutputW(m_console_handle, temp.data(), { (SHORT)getSize().x, (SHORT)getSize().y }, { 0,0 }, &srect);	

			//reset
			SetConsoleCursorPosition(m_console_handle, { 0,0 });
			SetConsoleTextAttribute(m_console_handle, getRgbiColor({ COLOR::WHITE, COLOR::BLACK }));
#endif

#ifdef TUI_TARGET_SYSTEM_LINUX
			static const std::array<std::string, 16> bg =
			{
				{ "40","44","42","46","41","45","43","47","100","104","102","106","101","105","103","107" }
			};

			static const std::array<std::string, 16> fg =
			{
				{ "30","34","32","36","31","35","33","37","30;1","34;1","32;1","36;1","31;1","35;1","33;1","37;1" }
			};
			auto getEscCodeRgbi = [&](color c)
			{
				std::string esc_c = "\033[";
				esc_c += fg[rgbToRgbi(c.foreground)];
				esc_c += ";";
				esc_c += bg[rgbToRgbi(c.background)];
				esc_c += "m";

				return esc_c;
			};
			auto getEscCodeRgb = [&](color c)
			{
				std::string esc_c = "\033[38;2;";
				esc_c += std::to_string(c.foreground.r);
				esc_c += ";";
				esc_c += std::to_string(c.foreground.g);
				esc_c += ";";
				esc_c += std::to_string(c.foreground.b);
				esc_c +="m";

				esc_c += "\033[48;2;";
				esc_c += std::to_string(c.background.r);
				esc_c += ";";
				esc_c += std::to_string(c.background.g);
				esc_c += ";";
				esc_c += std::to_string(c.background.b);
				esc_c += "m";

				return esc_c;
			};
			
			std::string str;

			color last_color;
			bool last_underscore = false;

			for (int i = 0; i < getSize().y; i++)
			{
				for (int j = 0; j < getSize().x; j++)
				{
					if (last_color != m_buffer.getSymbolAt({ j, i }).getColor())
					{
						if (m_display_rgbi)
						{
							str += getEscCodeRgbi(m_buffer.getSymbolAt({ j, i }).getColor());
						}
						if (m_display_rgb)
						{
							str += getEscCodeRgb(m_buffer.getSymbolAt({ j, i }).getColor());
						}
					}
					last_color = m_buffer.getSymbolAt({ j, i }).getColor();

					switch(m_buffer.getSymbolAt({ j, i }).isUnderscore())
					{
					case true:
						if (last_underscore) { break; }
						str += "\033[4m";
						break;
					case false:
						if (!last_underscore) { break; }
						str += "\033[24m";
					}
					last_underscore = m_buffer.getSymbolAt({ j, i }).isUnderscore();

					if (m_buffer.getSymbolAt({ j, i }).getFirstChar() >= 32)
					{
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
			std::cout << str << "\033[H" << str << "\033[H" << getEscCodeRgbi({ COLOR::WHITE, COLOR::BLACK });;
#endif
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

			m_resized = console_size.x != m_last_size.x || console_size.y != m_last_size.y;
			if (m_resized)
			{
				resize(console_size);
			}

			m_last_size = console_size;
		}

		void hidePrompt()
		{
#ifdef TUI_TARGET_SYSTEM_WINDOWS
			CONSOLE_CURSOR_INFO cursor_info;
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