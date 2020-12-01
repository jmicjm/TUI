#include "tui_input.h"
#include "tui_output.h"
#include "tui_terminal_info.h"
#include "tui_text_utils.h"
#include "tui_time_frame.h"

#include <cstdlib>
#include <vector>
#include <array>
#include <string>
#include <iostream>

#if defined(_WIN32)
	#include <windows.h>
#endif

#if defined(__linux__) || defined(__unix__) 
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

			void clearBuffer(rgb c) { buffer.fill({ ' ', {c,c} }); }

			vec2i getSize() { return buffer.getSize(); }

			bool isResized() { return buffer.isResized(); }

			void draw(surface& surf, bool update = true) { buffer.insertSurface(surf, update); }

			void updateSurfaceSize(surface& surf) { buffer.updateSurfaceSize(surf); }

			void updateSurfacePosition(surface& surf) { buffer.updateSurfacePosition(surf); }
		};

		struct console : console_buffer
		{
#if defined(_WIN32)
			HANDLE console_handle;
#endif
			time_frame fps_control;
			bool display_rgb = true;
			bool display_rgbi = true;

			console() : fps_control(std::chrono::milliseconds(1000) / 30)
			{
#if defined(_WIN32)
				console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
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

				auto getIColor = [](const symbol& s)
				{
					color c = s.getColor();
					if (s.isInverted())
					{
						c.invert();
					}
					return c;
				};

#if defined(_WIN32)
				std::vector<CHAR_INFO> temp(getSize().x * getSize().y);

				auto getRgbiColor = [&](color c)
				{
					return 16 * rgbToRgbi(c.background) + rgbToRgbi(c.foreground);
				};

				for (int y = 0; y < getSize().y; y++)
				{
					for (int x = 0; x < getSize().x;)
					{
						CHAR_INFO ch_info;
						uint8_t sym_w = buffer[x][y].getWidth();

						char32_t c = utf8ToUtf32(buffer[x][y].getCluster())[0];

						if (c < pow(2, (sizeof(wchar_t) * 8)) && sym_w != 0)
						{
							uint8_t sym_w = buffer[x][y].getWidth();

							ch_info.Char.UnicodeChar = c;
						}
						else { ch_info.Char.UnicodeChar = U'\xFFFD'; }

						if (display_rgbi)
						{
							ch_info.Attributes = getRgbiColor(getIColor(buffer[x][y]));
						}
						else
						{
							ch_info.Attributes = getRgbiColor({ COLOR::WHITE, COLOR::BLACK });
						}

						if (buffer[x][y].isUnderscore())
						{
							ch_info.Attributes |= COMMON_LVB_UNDERSCORE;
						}

						temp[y*getSize().x + x] = ch_info;

						if (sym_w > 1)
						{
							CHAR_INFO filler_ch;
							filler_ch.Attributes = ch_info.Attributes;
							filler_ch.Char.UnicodeChar = ' ';

							for (int i = 1; i < sym_w && x + i < getSize().x; i++)
							{
								temp[y * getSize().x + x + i] = filler_ch;
							}
						}

						x += sym_w > 0 ? sym_w : 1;
					}
				}

				SetConsoleCursorPosition(console_handle, { 0,0 });//without this output may be misplaced after console resize if "wrap text on resize" was selected in console options

				SMALL_RECT srect = { 0,0,(SHORT)getSize().x, (SHORT)getSize().y };
				WriteConsoleOutputW(console_handle, temp.data(), { (SHORT)getSize().x, (SHORT)getSize().y }, { 0,0 }, &srect);
#endif

#if defined(__linux__) || defined(__unix__) 
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

				std::string str = "\033[24m";
				
				color f_color = getIColor(buffer[0][0]);
				if (display_rgbi)
				{
					str += getEscCodeRgbi(f_color);
				}
				if (display_rgb)
				{
					str += getEscCodeRgb(f_color);
				}

				color last_color = f_color;
				bool last_underscore = false;

				for (int y = 0; y < getSize().y; y++)
				{
					for (int x = 0; x < getSize().x;)
					{
						uint8_t sym_w = buffer[x][y].getWidth();

						if (x + sym_w > getSize().x)
						{
							str += '\n';
							break;
						}

						color c_color = getIColor(buffer[x][y]);
						if (last_color != c_color)
						{
							if (display_rgbi)
							{
								str += getEscCodeRgbi(c_color);
							}
							if (display_rgb)
							{
								str += getEscCodeRgb(c_color);
							}
						}
						last_color = c_color;

						switch (buffer[x][y].isUnderscore())
						{
						case true:
							if (last_underscore) { break; }
							str += "\033[4m";
							break;
						case false:
							if (!last_underscore) { break; }
							str += "\033[24m";
						}
						last_underscore = buffer[x][y].isUnderscore();

						if (!isControl(utf8ToUtf32(buffer[x][y].getCluster())[0]))
						{
							str += buffer[x][y].getCluster();
						}
						else
						{
							str += "\xEF\xBF\xBD";
						}

						
						x += sym_w > 0 ? sym_w : 1;
					}
				}

				/* if not doubled last few characters will be displayed after
				some delay. I dont know why this happen*/
				std::cout << term_info.home << str << term_info.home << str;
#endif
				hidePrompt();
				updateLastBuffer();
			}

			void updateSize()
			{
				vec2i console_size;

#if defined(_WIN32)
				CONSOLE_SCREEN_BUFFER_INFO buffer_info;
				GetConsoleScreenBufferInfo(console_handle, &buffer_info);

				console_size.x = buffer_info.srWindow.Right - buffer_info.srWindow.Left + 1;
				console_size.y = buffer_info.srWindow.Bottom - buffer_info.srWindow.Top + 1;
#endif

#if defined(__linux__) || defined(__unix__) 
				winsize w;
				ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

				console_size.x = w.ws_col;
				console_size.y = w.ws_row;
#endif
				resize(console_size);
			}

			void hidePrompt()
			{
#if defined(_WIN32)
				CONSOLE_CURSOR_INFO cursor_info;
				cursor_info.bVisible = false;
				cursor_info.dwSize = 1;

				SetConsoleCursorInfo(console_handle, &cursor_info);
#endif

#if defined(__linux__) || defined(__unix__) 
				std::cout << term_info.civis;
#endif
			}
		} con;

		void clear(rgb c) 
		{
			con.updateSize();
			con.clearBuffer(c);
		}

		void draw(surface& surf, bool update) 
		{
			con.buffer.insertSurface(surf, update); 
		}
		void draw(surface& surf, surface::color_override c_override, bool update)
		{
			con.buffer.insertSurface(surf, c_override, update);
		}
		void draw(surface& surf, surface::color_transparency_override c_t_override, bool update)
		{
			con.buffer.insertSurface(surf, c_t_override, update);
		}
		void draw(
			surface& surf,
			surface::color_override c_override,
			surface::color_transparency_override c_t_override,
			bool update
		)
		{
			con.buffer.insertSurface(surf, c_override, c_t_override, update);
		}

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


		void clearDisplay()
		{
#if defined(_WIN32)
			CHAR_INFO i;
			i.Char.UnicodeChar = ' ';
			i.Attributes = 0;
			std::vector<CHAR_INFO> temp(getSize().x * getSize().y, i);
			SMALL_RECT srect = { 0,0,(SHORT)getSize().x, (SHORT)getSize().y };
			WriteConsoleOutputW(con.console_handle, temp.data(), { (SHORT)getSize().x, (SHORT)getSize().y }, { 0,0 }, &srect);
#endif
#if defined(__linux__) || defined(__unix__) 
			std::string str(getSize().x * getSize().y, ' ');
			std::cout << "\033[0m" << str << term_info.home;
#endif
		}

		void restore()
		{
#if defined(_WIN32)
			CONSOLE_CURSOR_INFO cursor_info;
			cursor_info.bVisible = true;
			cursor_info.dwSize = 1;

			SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
#endif

#if defined(__linux__) || defined(__unix__) 
			std::cout << term_info.cvvis;
#endif
			clearDisplay();
		}

		void init()
		{
#if defined(_WIN32)
			system("chcp 65001");		
#endif
			std::atexit(restore);

			con.hidePrompt();

			displayRgbColor(term_info.rgb_color);
		}
	}
}