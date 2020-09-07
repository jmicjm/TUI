#include "tui_input.h"
#include "tui_terminal_info.h"
#include "tui_unicode_utils.h"

#include <thread>
#include <mutex>
#include <algorithm>
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstdio>
#include <cmath>

#if defined(_WIN32)
	#include <conio.h>
#endif

#if defined(__linux__) || defined(__unix__) 
	#include "termios.h"
	#include "unistd.h" 
#endif

namespace tui
{
	namespace input
	{
		void CtrlcHandler()
		{
			std::exit(0);
		}

#if defined(__linux__) || defined(__unix__) 
		termios default_settings;
		termios noncanon_settings;
		termios nonblocking_settings;
#endif
#if defined(_WIN32)
		bool nonblocking = false;
#endif

		void useNonCanon()
		{
#if defined(__linux__) || defined(__unix__) 
			tcsetattr(0, TCSANOW, &noncanon_settings);
#endif
#if defined(_WIN32)
			nonblocking = false;
#endif
		}
		void useNonBlocking()
		{
#if defined(__linux__) || defined(__unix__) 
			tcsetattr(0, TCSANOW, &nonblocking_settings);
#endif
#if defined(_WIN32)
			nonblocking = true;
#endif
		}

		int gchar()
		{
#if defined(_WIN32)
			switch (nonblocking)
			{
			case false:
				return _getch();
			case true:
				if (_kbhit())
				{
					return _getch();
				}
				else
				{
					return -1;
				}
			}
#endif
#if defined(__linux__) || defined(__unix__) 
			char c;
			size_t s = read(STDIN_FILENO, &c, 1);

			if (s > 0)
			{
				return c;
			}
			else
			{
				return -1;
			}
#endif
		}

		unsigned int expectedUtf8Len(char c)
		{
			if ((c & 0b10000000) == 0b00000000)
			{
				return 0;
			}
			else if ((c & 0b11100000) == 0b11000000)
			{
				return 1;
			}
			else if ((c & 0b11110000) == 0b11100000)
			{
				return 2;
			}
			else if ((c & 0b11111000) == 0b11110000)
			{
				return 3;
			}

			return 0;
		}

		struct keyboard_buffer
		{
		private:
			std::mutex m_mtx;
		public:
			std::string raw[2];
			std::string str[2];
			std::vector<short> input[2];

			bool running = false;
			bool terminate_req = false;
			std::mutex sync_mtx;

			void bufferThread()
			{
				auto push = [&](unsigned char ch)
				{
					input[1].push_back(ch);

					if (ch >= 32 && ch != 127 && ch <= 255)
					{
						str[1] += ch;
					}
				};
				auto pushRaw = [&](int ch)
				{
					if (ch != -1)
					{
						raw[1] += ch;
					}
				};

				for (;;)
				{
					sync_mtx.lock();
					if (terminate_req)
					{
						running = false;
						terminate_req = false;
						clear();
#if defined(__linux__) || defined(__unix__) 
						tcsetattr(0, TCSANOW, &default_settings);
#endif
						sync_mtx.unlock();
						return;
					}
					sync_mtx.unlock();


					int gc = gchar();
					m_mtx.lock();
					pushRaw(gc);

					if (gc == CTRL_C)
					{
						CtrlcHandler();
					}

#if defined(_WIN32)
					if (gc != 0 && gc != 224)
					{
						push(gc);
					}
					else//non-alphanumeric, consisting of more than one byte
					{
						useNonBlocking();
						int gc2 = gchar();
						raw[1] += gc2;
						useNonCanon();

						switch (gc2 != -1)
						{
						case true:
							if (term_info.getSeqNumber({gc, gc2 }) >= 0)
							{
								input[1].push_back(term_info.getSeqNumber({gc, gc2 }) + TUI_KEY_OFFSET);
							}
							else
							{
								push(gc);
								push(gc2);
							}
							break;
						case false:
							push(gc);
						}
					}
#endif
#if defined(__linux__) || defined(__unix__) 
					if (gc == 127)//backspace could be 8 or 127
					{
						input[1].push_back(BACKSPACE);
					}
					else if (gc != 27)
					{
						push(gc);
					}
					else//non-alphanumeric, consisting of more than one byte
					{
						useNonBlocking();
						std::vector<int> buf = { 27 };

						auto copyToBuffer = [&]()
						{
							for (int j = 0; j < buf.size(); j++)
							{
								push(buf[j]);
							}
						};

						for (int i = 0; i < term_info.longest_seq; i++)
						{
							int gcn = gchar();
							pushRaw(gcn);

							if (gcn == -1)//no input
							{
								copyToBuffer();
								break;
							}
							else
							{
								buf.push_back(gcn);
							}

							if (term_info.getSeqNumber(buf) >= 0)
							{
								input[1].push_back(term_info.getSeqNumber(buf) + TUI_KEY_OFFSET);
								break;
							}

							if (i == term_info.longest_seq - 1)
							{
								copyToBuffer();
								break;
							}
						}
						useNonCanon();
					}
#endif
					char utf8_buf[3] = { 0,0,0 };
					unsigned int buf_len = 0;

					useNonBlocking();
					for (buf_len = 0; buf_len < expectedUtf8Len(gc); buf_len++)
					{
						int c = gchar();
						pushRaw(c);
						if (c != -1)
						{
							utf8_buf[buf_len] = c;
						}
						else
						{
							break;
						}
					}
					useNonCanon();

					for (int i = 0; i < buf_len; i++)
					{
						push(utf8_buf[i]);
					}


					m_mtx.unlock();
				}
			}

			void swap()
			{
				m_mtx.lock();

				raw[0] = raw[1];
				str[0] = str[1];
				input[0] = input[1];

				raw[1].clear();
				str[1].clear();
				input[1].clear();

				m_mtx.unlock();
			}

			void clear()
			{
				m_mtx.lock();

				raw[0].clear();
				raw[1].clear();
				str[0].clear();
				str[1].clear();
				input[0].clear();
				input[1].clear();

				m_mtx.unlock();
			}
		};
		keyboard_buffer buffer;

		void restore()
		{
			buffer.sync_mtx.lock();
			if (buffer.running)
			{
				buffer.terminate_req = true;
			}
			buffer.sync_mtx.unlock();

#if defined(__linux__) || defined(__unix__) 
			tcsetattr(0, TCSANOW, &default_settings);
			std::cout << term_info.rmkx;
#endif
		}

		void init()
		{
#if defined(__linux__) || defined(__unix__) 
			tcgetattr(0, &default_settings);

			noncanon_settings = default_settings;
			noncanon_settings.c_lflag &= ~(ICANON | ECHO | ISIG | IEXTEN);
			noncanon_settings.c_iflag &= ~(ICRNL | IXON);

			nonblocking_settings = noncanon_settings;
			nonblocking_settings.c_cc[VMIN] = 0;
			nonblocking_settings.c_cc[VTIME] = 0;

			tcsetattr(0, TCSANOW, &noncanon_settings);
			std::cout << term_info.smkx;
#endif	

			std::atexit(restore);

			buffer.sync_mtx.lock();

			switch (buffer.running)
			{
			case true:
				buffer.terminate_req = false;
				break;
			case false:
				std::thread keyboardBufferThread([&] {buffer.bufferThread(); });
				keyboardBufferThread.detach();
				buffer.running = true;
			}

			buffer.sync_mtx.unlock();
		}

		std::vector<short> getInput()
		{
			return buffer.input[0];
		}
		std::string getRawInput()
		{
			return buffer.raw[0];
		}
		std::string getStringInput()
		{
			return buffer.str[0];
		}

		int isKeyPressed(short key)
		{
			if (key >= 0)
			{
				std::vector<short>& input = buffer.input[0];
				return std::count(input.begin(), input.end(), key);

			}
			return false;
		}

		int isCodePointPressed(char32_t code_point)
		{
			std::u32string u32_str = utf8ToUtf32(buffer.str[0]);
			return std::count(u32_str.begin(), u32_str.end(), code_point);
		}

		void swap() { buffer.swap(); }

		void clear() { buffer.clear(); }

		bool isKeySupported(short key)
		{
			if (key >= 0 && key <= 255) { return true; }
			else if (key >= KEY::PGUP && key <= KEY::CTRL_RIGHT)
			{
#if defined(__linux__) || defined(__unix__) 
				return term_info.getSeq(key - TUI_KEY_OFFSET).size() > 0;
#endif
#if defined(_WIN32)
				if (!(key >= KEY::SHIFT_UP && key <= KEY::SHIFT_RIGHT) && key != KEY::CTRL_PGUP)
				{
					return term_info.getSeq(key - TUI_KEY_OFFSET).size() > 0;
				}
#endif
			}
			return false;	
		}

		std::string getKeyName(short key, bool use_ctrl_name)
		{
			std::string name;

			if (key > 32 && key < 128)
			{
				name = key;
			}
			else if (key == 32)
			{
				name = "SPACE";
			}
			else if (key > 0 && key < 32)
			{
				name = std::string("CTRL+") + char(key+64);
			}

			if (!use_ctrl_name)
			{
				switch (key)
				{
				case BACKSPACE:
					name = "BACKSPACE";
					break;
				case TAB:
					name = "TAB";
					break;
				case ENTER:
					name = "ENTER";
					break;
				case ESC:
					name = "ESC";
				}
			}

			static const std::string offset_key[70] = {
				"PGUP","PGDN","DEL","INS","END","HOME",
				"F1","F2","F3","F4","F5","F6",
				"F7","F8","F9","F10","F11","F12",
				"UP","DOWN","LEFT","RIGHT",

				"ALT+PGUP","ALT+PGDN","ALT+DEL","ALT+INS","ALT+END","ALT+HOME",
				"ALT+UP","ALT+DOWN","ALT+LEFT","ALT+RIGHT",

				"SHIFT+F1","SHIFT+F2","SHIFT+F3","SHIFT+F4","SHIFT+F5","SHIFT+F6",
				"SHIFT+F7","SHIFT+F8","SHIFT+F9","SHIFT+F10","SHIFT+F11","SHIFT+F12",
				"SHIFT+UP","SHIFT+DOWN","SHIFT+LEFT","SHIFT+RIGHT",

				"CTRL+PGUP","CTRL+PGDN","CTRL+DEL","CTRL+INS","CTRL+END","CTRL+HOME",
				"CTRL+F1","CTRL+F2","CTRL+F3","CTRL+F4","CTRL+F5","CTRL+F6",
				"CTRL+F7","CTRL+F8","CTRL+F9","CTRL+F10","CTRL+F11","CTRL+F12",
				"CTRL+UP","CTRL+DOWN","CTRL+LEFT","CTRL+RIGHT"
			};

			if (key >= KEY::PGUP && key <= KEY::CTRL_RIGHT)
			{
				name = offset_key[key - TUI_KEY_OFFSET];
			}

			return name;
		}

	}
}