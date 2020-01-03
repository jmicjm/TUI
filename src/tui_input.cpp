#include "tui_input.h"
#include "tui_terminal_info.h"

#include <thread>
#include <mutex>
#include <cstdlib>
#include <cstdio>
#include <algorithm>
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

		void restoreTerminal()
		{
#ifdef  TUI_TARGET_SYSTEM_LINUX
			tcsetattr(0, TCSANOW, &default_settings);
			system("tput rmkx");
#endif
		}

		terminal_info term_info;

		struct keyboard_buffer
		{
		private:
			std::mutex m_mtx;
		public:
			std::string m_raw[2];
			std::string m_str[2];
			std::vector<short> m_input[2];

			keyboard_buffer()
			{
#ifdef  TUI_TARGET_SYSTEM_LINUX
				tcgetattr(0, &default_settings);

				std::atexit(restoreTerminal);

				noncanon_settings = default_settings;
				noncanon_settings.c_lflag &= ~(ICANON | ECHO | ISIG | IEXTEN);
				noncanon_settings.c_iflag &= ~(ICRNL | IXON);

				nonblocking_settings = noncanon_settings;
				nonblocking_settings.c_cc[VMIN] = 0;
				nonblocking_settings.c_cc[VTIME] = 0;

				tcsetattr(0, TCSANOW, &noncanon_settings);
				system("tput smkx");
#endif
				std::thread keyboardBufferThread([this] {bufferThread(); });
				keyboardBufferThread.detach();
			}

			~keyboard_buffer()
			{
				restoreTerminal();
			}

			void bufferThread()
			{
				auto gchar = [&]()
				{
#ifdef  TUI_TARGET_SYSTEM_WINDOWS
					return _getch();
#endif
#ifdef TUI_TARGET_SYSTEM_LINUX
					return getchar();
#endif
				};

				auto push = [&](unsigned char ch)
				{
					m_input[1].push_back(ch);

					if (ch >= 32 && ch != 127 && ch <= 255)
					{
						m_str[1] += ch;
					}
				};

				for (;;)
				{
					int gc = gchar();
					m_mtx.lock();
					m_raw[1] += gc;

					if (gc == CTRL_C)
					{
						CtrlcHandler();
					}
					if (gc == 10 || gc == 13) { m_str[1] += '\n'; }

#ifdef  TUI_TARGET_SYSTEM_WINDOWS
					if (gc != 0 && gc != 224)
					{
						push(gc);
					}
					else//non-alphanumeric, consisting of more than one byte
					{
						int gc2 = gchar();
						m_raw[1] += gc2;

						if (term_info.getSeqNumber({ gc2 }) >= 0)
						{
							m_input[1].push_back(term_info.getSeqNumber({ gc2 }) + TUI_KEY_OFFSET);
						}
					}
#endif
#ifdef TUI_TARGET_SYSTEM_LINUX
					if (gc == 127)//backspace could be 8 or 127
					{
						m_input[1].push_back(BACKSPACE);
					}

					if (gc != 27)
					{
						push(gc);
					}
					else//non-alphanumeric, consisting of more than one byte
					{
						tcsetattr(0, TCSANOW, &nonblocking_settings);
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
							m_raw[1] += gcn;

							if (gcn == -1)//no input
							{
								copyToBuffer();

								tcsetattr(0, TCSANOW, &noncanon_settings);
								break;
							}
							else
							{
								buf.push_back(gcn);
							}

							if (term_info.getSeqNumber(buf) >= 0)
							{
								m_input[1].push_back(term_info.getSeqNumber(buf) + TUI_KEY_OFFSET);

								tcsetattr(0, TCSANOW, &noncanon_settings);
								break;
							}

							if (i == term_info.longest_seq - 1)
							{
								copyToBuffer();

								tcsetattr(0, TCSANOW, &noncanon_settings);
								break;
							}
						}
					}
#endif
					m_mtx.unlock();
				}
			}

			void swap()
			{
				m_mtx.lock();

				m_raw[0] = m_raw[1];
				m_str[0] = m_str[1];
				m_input[0] = m_input[1];

				m_raw[1].clear();
				m_str[1].clear();
				m_input[1].clear();

				m_mtx.unlock();
			}

			void clear()
			{
				m_mtx.lock();

				m_raw[0].clear();
				m_raw[1].clear();
				m_str[0].clear();
				m_str[1].clear();
				m_input[0].clear();
				m_input[1].clear();

				m_mtx.unlock();
			}
		};
		keyboard_buffer buffer;

		std::vector<short> getInput()
		{
			return buffer.m_input[0];
		}
		std::string getRawInput()
		{
			return buffer.m_raw[0];
		}
		std::string getStringInput()
		{
			return buffer.m_str[0];
		}

		int isKeyPressed(short key)
		{
			if (key >= 0)
			{
				std::vector<short>& input = buffer.m_input[0];
				return std::count(input.begin(), input.end(), key);

			}
			return false;
		}

		void swap() { buffer.swap(); }

		void clear() { buffer.clear(); }

		bool isKeySupported(short key)
		{
			if (key < 0) { return false; }
			if (key >= 0 && key <= 256) { return true; }
			else if (key >= 256)
			{
				return term_info.getSeq(key - TUI_KEY_OFFSET).size() > 0;
			}
		}
	}
}