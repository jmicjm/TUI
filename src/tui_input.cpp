#include "tui_input.h"

#include <thread>
#include <cstdio>
#include <algorithm>
#include <mutex>
#include <cmath>

namespace tui
{
	namespace input
	{
		void CTRLC_handler()
		{

		}

#ifdef  TUI_TARGET_SYSTEM_LINUX
		struct key_seq_pair
		{
			std::string name;
			std::vector<int> seq;
			key_seq_pair() {}
			key_seq_pair(std::string Name) : name(Name) {}
			key_seq_pair(std::string Name, std::vector<int> Seq) : name(Name), seq(Seq) {}
		};



		struct terminal_info
		{
			terminal_info()
			{
				std::array<char, 128> buffer;
				std::string infocmp;
				FILE* pipe = popen("infocmp", "r");
				if (pipe)
				{
					while (fgets(buffer.data(), buffer.size(), pipe) != NULL) {
						infocmp += buffer.data();
					}
				}
				pclose(pipe);

				int longest = 0;
				int shortest = pow(2, sizeof(int) * 8) - 1;

				for (int i = 0; i < sequences.size(); i++)
				{
					if (infocmp.find(sequences[i].name) != std::string::npos)
					{
						int s = infocmp.find(sequences[i].name);
						s += sequences[i].name.size();
						s += 3; // =\E

						sequences[i].seq = { 27 }; //ESC

						while (infocmp[s] != ',')
						{
							sequences[i].seq.push_back(infocmp[s]);
							s++;
						}
					}
					else
					{
						sequences[i].seq = { -2 };
					}

					if (sequences[i].seq.size() > longest)
					{
						longest = sequences[i].seq.size();
					}
					if (sequences[i].seq.size() < shortest)
					{
						shortest = sequences[i].seq.size();
					}
				}

				longest_seq = longest;
				shortest_seq = shortest;


				if (infocmp.find("smkx") != std::string::npos)
				{
					int s = infocmp.find("smkx");
					s += 7; //smkx=\E
					smkx = '\033';

					while (infocmp[s] != ',')
					{
						smkx += infocmp[s];
						s++;
					}
				}
				if (infocmp.find("rmkx") != std::string::npos)
				{
					int s = infocmp.find("rmkx");
					s += 7; //rmkx=\E
					rmkx = '\033';

					while (infocmp[s] != ',')
					{
						rmkx += infocmp[s];
						s++;
					}
				}

			}

			std::vector<key_seq_pair> sequences =
			{
				{"kpp"}, //PGUP
				{"knp"}, //PGDN

				{"kdch1"}, //DELETE
				{"kich1"}, //INSERT
				{"kend"}, //END
				{"khome"}, //HOME

				{"kf1"}, //F1
				{"kf2"},
				{"kf3"},
				{"kf4"},
				{"kf5"},
				{"kf6"},
				{"kf7"},
				{"kf8"},
				{"kf9"},
				{"kf10"},
				{"kf11"},
				{"kf12"}, //F12

				{"kcuu1"}, //UP
				{"kcud1"}, //DOWN
				{"kcub1"}, //LEFT
				{"kcuf1"} //RIGHT
			};


			std::string smkx;
			std::string rmkx;

			int longest_seq;
			int shortest_seq;

			//return position of sequence, if there is no given sequence return -1
			int getSeqNumber(std::vector<int> seq)
			{
				for (int i = 0; i < sequences.size(); i++)
				{
					if (sequences[i].seq == seq)
					{
						return i;
					}
				}
				return -1;
			}

		};
		terminal_info term_info;
#endif

		struct keyboard_buffer
		{
		private:
#ifdef  TUI_TARGET_SYSTEM_LINUX
			termios default_settings;
			termios noncanon_settings;
			termios nonblocking_settings;
#endif
			std::string m_raw[2];
			std::string m_str[2];
			std::vector<short> m_input[2];

			std::mutex m_mtx;

			friend std::string GetRawInput();
			friend std::string GetStringInput();
			friend std::vector<short> GetInput();
			friend int IsKeyPressed(short);
		public:
			keyboard_buffer()
			{
#ifdef  TUI_TARGET_SYSTEM_LINUX
				tcgetattr(0, &default_settings);

				noncanon_settings = default_settings;
				noncanon_settings.c_lflag &= ~ICANON;
				noncanon_settings.c_lflag &= ~ECHO;
				noncanon_settings.c_iflag &= ~ICRNL;

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
#ifdef  TUI_TARGET_SYSTEM_LINUX
				tcsetattr(0, TCSANOW, &default_settings);
				system("tput rmkx");
#endif
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

					if (gc == 3)
					{
						CTRLC_handler();
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

						if (gc2 >= 0 && gc2 <= 255)
						{
							m_input[1].push_back(gc2 + TUI_KEY_OFFSET);
						}
					}
#endif
#ifdef TUI_TARGET_SYSTEM_LINUX
					if (gc == 8)//backspace could be 8 or 127(BACKSPACE ENUM)
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

		std::vector<short> GetInput()
		{
			return buffer.m_input[0];
		}
		std::string GetRawInput()
		{
			return buffer.m_raw[0];
		}
		std::string GetStringInput()
		{
			return buffer.m_str[0];
		}

		int IsKeyPressed(short key)
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
	}
}