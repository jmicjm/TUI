#pragma once
#include <string>
#include <thread>
#include <chrono>
#include <vector>
#include <cmath>
#include <cstdio>


#ifdef  TUI_TARGET_SYSTEM_WINDOWS
	#include <conio.h>
#endif

#ifdef  TUI_TARGET_SYSTEM_LINUX
#include "termios.h"
#endif

#define TUI_GETCH_RANGE 256

#define TUI_BUFFER_OFFSET 0xF00 // could be any value above TUI_GETCH_RANGE




namespace tui
{
	namespace input
	{


		inline void CTRLC_handler()
		{

		}

#ifdef  TUI_TARGET_SYSTEM_WINDOWS
		enum KEY
		{
			ENTER = 13,
			BACKSPACE = 8,
			ESC = 27,
			TAB = 9,

			PGUP = TUI_BUFFER_OFFSET + 73,
			PGDN = TUI_BUFFER_OFFSET + 81,

			DEL = TUI_BUFFER_OFFSET + 83,
			INS = TUI_BUFFER_OFFSET + 82,
			END = TUI_BUFFER_OFFSET + 79,
			HOME = TUI_BUFFER_OFFSET + 71,

			F1 = TUI_BUFFER_OFFSET + 59,
			F2 = TUI_BUFFER_OFFSET + 60,
			F3 = TUI_BUFFER_OFFSET + 61,
			F4 = TUI_BUFFER_OFFSET + 62,
			F5 = TUI_BUFFER_OFFSET + 63,
			F6 = TUI_BUFFER_OFFSET + 64,
			F7 = TUI_BUFFER_OFFSET + 65,
			F8 = TUI_BUFFER_OFFSET + 66,
			F9 = TUI_BUFFER_OFFSET + 67,
			F10 = TUI_BUFFER_OFFSET + 68,
			F11 = TUI_BUFFER_OFFSET + 133,
			F12 = TUI_BUFFER_OFFSET + 134,

			UP = TUI_BUFFER_OFFSET + 72,
			DOWN = TUI_BUFFER_OFFSET + 80,
			LEFT = TUI_BUFFER_OFFSET + 75,
			RIGHT = TUI_BUFFER_OFFSET + 77

		};
#endif

#ifdef  TUI_TARGET_SYSTEM_LINUX
		struct key_seq_pair
		{
			std::string name;
			std::vector<int> seq;
			key_seq_pair() {}
			key_seq_pair(std::string Name) : name(Name) {}
			key_seq_pair(std::string Name, std::vector<int> Seq) : name(Name), seq(Seq) {}
		};

		enum KEY
		{
			ENTER = 13,
			BACKSPACE = 127,
			ESC = 27,
			TAB = 9,

			PGUP = TUI_BUFFER_OFFSET,
			PGDN = TUI_BUFFER_OFFSET + 1,

			DEL = TUI_BUFFER_OFFSET + 2,
			INS = TUI_BUFFER_OFFSET + 3,
			END = TUI_BUFFER_OFFSET + 4,
			HOME = TUI_BUFFER_OFFSET + 5,

			F1 = TUI_BUFFER_OFFSET + 6,
			F2 = TUI_BUFFER_OFFSET + 7,
			F3 = TUI_BUFFER_OFFSET + 8,
			F4 = TUI_BUFFER_OFFSET + 9,
			F5 = TUI_BUFFER_OFFSET + 10,
			F6 = TUI_BUFFER_OFFSET + 11,
			F7 = TUI_BUFFER_OFFSET + 12,
			F8 = TUI_BUFFER_OFFSET + 13,
			F9 = TUI_BUFFER_OFFSET + 14,
			F10 = TUI_BUFFER_OFFSET + 15,
			F11 = TUI_BUFFER_OFFSET + 16,
			F12 = TUI_BUFFER_OFFSET + 17,

			UP = TUI_BUFFER_OFFSET + 18,
			DOWN = TUI_BUFFER_OFFSET + 19,
			LEFT = TUI_BUFFER_OFFSET + 20,
			RIGHT = TUI_BUFFER_OFFSET + 21
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
				int shortest = pow(2, sizeof(int)*8) - 1;

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
		extern terminal_info term_info;


#endif
		
		
		struct keyboard_buffer
		{
		private:
#ifdef  TUI_TARGET_SYSTEM_LINUX
			termios default_settings;
			termios noncanon_settings;
			termios nonblocking_settings;
#endif

			std::vector<int> buffer[2];
			std::vector<int> second_buffer[2]; //buffer for "characters" that needs to return more than one time

			std::string string_buffer[2];
		public:
			int operator[](int i) 
			{
				if (i < TUI_BUFFER_OFFSET)
				{
					return buffer[0][i];
				}
				else
				{
					return second_buffer[0][i - TUI_BUFFER_OFFSET];
				}
			}
			std::string getString() { return string_buffer[0]; }
			int size() { return buffer[0].size(); }

			keyboard_buffer()
			{
#ifdef  TUI_TARGET_SYSTEM_LINUX
				tcgetattr(0, &default_settings);

				noncanon_settings = default_settings;
				noncanon_settings.c_lflag &= ~ICANON;
				noncanon_settings.c_lflag &= ~ECHO;

				nonblocking_settings = noncanon_settings;
				nonblocking_settings.c_cc[VMIN] = 0;
				nonblocking_settings.c_cc[VTIME] = 0;

				tcsetattr(0, TCSANOW, &noncanon_settings);

				system("tput smkx");
#endif


				buffer[0].resize(TUI_GETCH_RANGE);
				buffer[1].resize(TUI_GETCH_RANGE);
				second_buffer[0].resize(TUI_GETCH_RANGE);
				second_buffer[1].resize(TUI_GETCH_RANGE);
		
				std::thread keyboardBufferThread([this] {bufferThread(); });
				keyboardBufferThread.detach();
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

				for (;;)
				{
					int pressed = gchar();

					if (pressed == 3)
					{
						CTRLC_handler();
					}
					if (pressed == 10 || pressed == 13) { string_buffer[1] += '\n'; }

#ifdef  TUI_TARGET_SYSTEM_WINDOWS
					if (pressed != 0 && pressed != 224)
					{
						if (pressed >= 0 && pressed < TUI_GETCH_RANGE)
						{
							buffer[1][pressed]++;

							if (pressed >= 32 && pressed != 127)
							{
								string_buffer[1] += (char)pressed;
							}
						}
					}

					if (pressed == 0 || pressed == 224)
					{
						int second_getch = gchar();

						if (second_getch >= 0 && second_getch < TUI_GETCH_RANGE)
						{
							second_buffer[1][second_getch]++;
						}
					}
#endif
#ifdef TUI_TARGET_SYSTEM_LINUX
					if (pressed == 127)//backspace could be 8 or 127
					{
						buffer[1][BACKSPACE]++;
					}

					if (pressed != 27)
					{
						if (pressed >= 0 && pressed < TUI_GETCH_RANGE)
						{
							buffer[1][pressed]++;

							if (pressed >= 32 && pressed != 127)
							{
								string_buffer[1] += (char)pressed;
							}
						}
					}
					else
					{
						tcsetattr(0, TCSANOW, &nonblocking_settings);
						std::vector<int> buf = { 27 };

						auto copyToBuffer = [&]()
						{
							for (int j = 0; j < buf.size(); j++)
							{
								if (buf[j] >= 0 && buf[j] < TUI_GETCH_RANGE)
								{
									buffer[1][buf[j]]++;

									if (buf[j] >= 32 && buf[j] != 127)
									{
										string_buffer[1] += (char)buf[j];
									}
								}
							}
						};

						
						for (int i = 0; i < term_info.longest_seq; i++)
						{
							int input = gchar();
							if (input == -1)
							{
								copyToBuffer();

								tcsetattr(0, TCSANOW, &noncanon_settings);
								break;
							}
							else
							{
								buf.push_back(input);
							}

							if (term_info.getSeqNumber(buf) >= 0)
							{
								second_buffer[1][term_info.getSeqNumber(buf)]++;

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
				}
			}

			//swaps buffers and clear [1] ones
			void clear()
			{
				buffer[0] = buffer[1];
				second_buffer[0] = second_buffer[1];
				string_buffer[0] = string_buffer[1];

				for (int i = 0; i < buffer[0].size(); i++)
				{
					buffer[1][i] = 0;
					second_buffer[1][i] = 0;
					string_buffer[1].clear();
				}
			}
		};
		extern keyboard_buffer buffer;

		//return amount of key press in iteration before last clear
		inline int IsKeyPressed(int key)
		{
			if (key >= 0) { return buffer[key]; }
			else { return false; }
		}

		//return string that consist of characters pressed in iteration before last clear
		inline std::string GetInputAsString()
		{
			return buffer.getString();
		}

		/*NOTE ABOUT DOUBLE BUFFERING

		X -> clear()
		abcd -> key presses

						   isKeyPressed() or getInputAsString() used here returns these characters
						    \/																	|
		|X|ab|cd|X|ab|cd|X|ab|cd|X|																|
				  /	    \																		|
				 |	     |<---------------------------------------------------------------------/

				without double buffering "cd" part will be lost		
		*/

	}
}
