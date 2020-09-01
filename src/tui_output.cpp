#include "tui_output.h"
#include "tui_input.h"
#include "tui_text_utils.h"
#include "tui_terminal_info.h"

#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>

#ifdef TUI_TARGET_SYSTEM_WINDOWS
	#include <windows.h>
#endif

#ifdef TUI_TARGET_SYSTEM_LINUX
	#include <sys/ioctl.h>
	#include <unistd.h>
	#include <termios.h>
#endif

namespace tui
{
	namespace output
	{
		struct console_buffer
		{
		protected:
			surface buffer;
			surface last_buffer;

			void resize(vec2i size) { buffer.setSizeInfo(size); }

			void updateLastBuffer() { last_buffer = buffer; }

			bool isChanged()
			{
				if (last_buffer.getSize() != buffer.getSize())
				{
					return true;
				}

				for (int y = 0; y < buffer.getSize().y; y++)
				{
					for (int x = 0; x < buffer.getSize().x; x++)
					{
						if (buffer[x][y] != last_buffer[x][y])
						{
							return true;
						}
					}
				}
				return false;
			}

			void clearBuffer() { buffer.makeBlank(); }
		public:
			vec2i getSize() { return buffer.getSize(); }

			bool isResized() { return buffer.isResized(); }

			void draw(surface& surf) { buffer.insertSurface(surf); }

			void updateSurfaceSize(surface& surf) { buffer.updateSurfaceSize(surf); }

			void updateSurfacePosition(surface& surf) { buffer.updateSurfacePosition(surf); }
		};

		struct console : console_buffer
		{
#ifdef TUI_TARGET_SYSTEM_WINDOWS
			HANDLE m_console_handle;
#endif
			time_frame fps_control;
			bool display_rgb = true;
			bool display_rgbi = true;

			console() : fps_control(std::chrono::milliseconds(1000) / 30)
			{
#ifdef  TUI_TARGET_SYSTEM_WINDOWS
				m_console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
			}

			void clear()
			{
				updateSize();
				clearBuffer();
			}

			void display()
			{
				fps_control.sleepUntilEnd();

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
					return (c1.r - c2.r) * (c1.r - c2.r) + (c1.g - c2.g) * (c1.g - c2.g) + (c1.b - c2.b) * (c1.b - c2.b);
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

						char32_t c = utf8ToUtf32(buffer[j][i].getCluster())[0];

						if (c < pow(2, (sizeof(wchar_t) * 8)) && !isControl(c))
						{
							ch_info.Char.UnicodeChar = c;
						}
						else { ch_info.Char.UnicodeChar = '?'; }

						if (display_rgbi)
						{
							ch_info.Attributes = getRgbiColor(buffer[j][i].getColor());
						}
						else
						{
							ch_info.Attributes = getRgbiColor({ COLOR::WHITE, COLOR::BLACK });
						}

						if (buffer[j][i].isUnderscore())
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
					esc_c += "m";

					esc_c += "\033[48;2;";
					esc_c += std::to_string(c.background.r);
					esc_c += ";";
					esc_c += std::to_string(c.background.g);
					esc_c += ";";
					esc_c += std::to_string(c.background.b);
					esc_c += "m";

					return esc_c;
				};

				std::string str = "\033[24m" + getEscCodeRgbi({ COLOR::WHITE, COLOR::BLACK }) + getEscCodeRgb({ COLOR::WHITE, COLOR::BLACK });

				color last_color = {COLOR::WHITE, COLOR::BLACK};
				bool last_underscore = false;

				for (int i = 0; i < getSize().y; i++)
				{
					for (int j = 0; j < getSize().x; j++)
					{
						if (last_color != buffer[j][i].getColor())
						{
							if (display_rgbi)
							{
								str += getEscCodeRgbi(buffer[j][i].getColor());
							}
							if (display_rgb)
							{
								str += getEscCodeRgb(buffer[j][i].getColor());
							}
						}
						last_color = buffer[j][i].getColor();

						switch (buffer[j][i].isUnderscore())
						{
						case true:
							if (last_underscore) { break; }
							str += "\033[4m";
							break;
						case false:
							if (!last_underscore) { break; }
							str += "\033[24m";
						}
						last_underscore = buffer[j][i].isUnderscore();

						if (!isControl(utf8ToUtf32(buffer[j][i].getCluster())[0]))
						{
							str += buffer[j][i].getCluster();
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
				resize(console_size);
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
		console con;

		void clear() { con.clear(); }

		void draw(surface& surf) { con.draw(surf); }

		void display() { con.display(); }

		vec2i getSize() { return con.getSize(); }

		bool isResized() { return con.isResized(); }

		void updateSurfaceSize(surface& surf) { con.updateSurfaceSize(surf); }

		void updateSurfacePosition(surface& surf) { con.updateSurfacePosition(surf); }

		void setFpslimit(unsigned int fps)
		{
			con.fps_control.setFrameTime(std::chrono::milliseconds(1000) / fps);
		}

		bool isTimeToDisplay()
		{
			return con.fps_control.isEnd(false);
		}

		void displayRgbColor(bool display) { con.display_rgb = display; }
		bool isDisplayingRgbColor() { return con.display_rgb; }

		void displayRgbiColor(bool display) { con.display_rgbi = display; }
		bool isDisplayingRgbiColor() { return con.display_rgbi; }

		void displayColor(bool display)
		{
			con.display_rgb = display;
			con.display_rgbi = display;
		}
		bool isDisplayingColor() { return con.display_rgbi || con.display_rgb; }

		void restore()
		{
#ifdef TUI_TARGET_SYSTEM_WINDOWS
			CONSOLE_CURSOR_INFO cursor_info;
			cursor_info.bVisible = true;
			cursor_info.dwSize = 1;

			SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
#endif

#ifdef TUI_TARGET_SYSTEM_LINUX
			std::cout << "\033[?25h";
#endif
		}

		void init()
		{
#ifdef  TUI_TARGET_SYSTEM_WINDOWS
			system("chcp 65001");		
#endif
			std::atexit(restore);

			con.hidePrompt();

			displayRgbColor(term_info.rgb_color);
		}
	}
}