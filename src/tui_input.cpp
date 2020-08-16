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

#ifdef  TUI_TARGET_SYSTEM_WINDOWS
	#include <conio.h>
#endif

#ifdef  TUI_TARGET_SYSTEM_LINUX
	#include "termios.h"
#endif

namespace tui
{
	namespace input
	{
		void CtrlcHandler()
		{
			std::exit(0);
		}

#ifdef  TUI_TARGET_SYSTEM_LINUX
		termios default_settings;
		termios noncanon_settings;
		termios nonblocking_settings;
#endif
#ifdef  TUI_TARGET_SYSTEM_WINDOWS
		bool nonblocking = false;
#endif

		void useNonCanon()
		{
#ifdef  TUI_TARGET_SYSTEM_LINUX
			tcsetattr(0, TCSANOW, &noncanon_settings);
#endif
#ifdef  TUI_TARGET_SYSTEM_WINDOWS
			nonblocking = false;
#endif
		}
		void useNonBlocking()
		{
#ifdef  TUI_TARGET_SYSTEM_LINUX
			tcsetattr(0, TCSANOW, &nonblocking_settings);
#endif
#ifdef  TUI_TARGET_SYSTEM_WINDOWS
			nonblocking = true;
#endif
		}

		int gchar()
		{
#ifdef  TUI_TARGET_SYSTEM_WINDOWS
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
#ifdef TUI_TARGET_SYSTEM_LINUX
			return getchar();
#endif
		};

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

				for (;;)
				{
					sync_mtx.lock();
					if (terminate_req)
					{
						running = false;
						terminate_req = false;
						clear();
						sync_mtx.unlock();
						return;
					}
					sync_mtx.unlock();


					int gc = gchar();
					m_mtx.lock();
					raw[1] += gc;

					if (gc == CTRL_C)
					{
						CtrlcHandler();
					}

#ifdef  TUI_TARGET_SYSTEM_WINDOWS
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
							if (term_info.getSeqNumber({ gc2 }) >= 0)
							{
								input[1].push_back(term_info.getSeqNumber({ gc2 }) + TUI_KEY_OFFSET);
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
#ifdef TUI_TARGET_SYSTEM_LINUX
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
							raw[1] += gcn;

							if (gcn == -1)//no input
							{
								copyToBuffer();

								useNonCanon();
								break;
							}
							else
							{
								buf.push_back(gcn);
							}

							if (term_info.getSeqNumber(buf) >= 0)
							{
								input[1].push_back(term_info.getSeqNumber(buf) + TUI_KEY_OFFSET);

								useNonCanon();
								break;
							}

							if (i == term_info.longest_seq - 1)
							{
								copyToBuffer();

								useNonCanon();
								break;
							}
						}
					}
#endif
					char utf8_buf[3] = { 0,0,0 };
					unsigned int buf_len = 0;

					useNonBlocking();
					for (buf_len = 0; buf_len < expectedUtf8Len(gc); buf_len++)
					{
						char c = gchar();
						raw[1].push_back(c);
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

#ifdef  TUI_TARGET_SYSTEM_LINUX
			tcsetattr(0, TCSANOW, &default_settings);
			std::cout << term_info.rmkx;
#endif
		}

		void init()
		{
#ifdef  TUI_TARGET_SYSTEM_LINUX
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
			else if (key >= KEY::PGUP && key <= KEY::RIGHT)
			{
				return term_info.getSeq(key - TUI_KEY_OFFSET).size() > 0;
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
				name = std::string("CTRL ") + char(key+64);
			}

			if (!use_ctrl_name)
			{
				switch (key)
				{
				case BACKSPACE:
					name = "BACKPSPACE";
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

			static const std::string offset_key[22] = {
				"PGUP","PGDN","DEL","INS","END","HOME",
				"F1","F2","F3","F4","F5","F6","F7","F8","F9","F10","F11","F12",
				"UP","DOWN","LEFT","RIGHT"
			};

			if (key >= TUI_KEY_OFFSET && key <= TUI_KEY_OFFSET+21)
			{
				name = offset_key[key - TUI_KEY_OFFSET];
			}

			return name;
		}

	}
}