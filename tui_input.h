#pragma once
#include <string>
#include <thread>
#include <chrono>
#include <vector>

#include "tui_config.h"


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
	namespace KEYBOARD
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

			PGUP = TUI_BUFFER_OFFSET + 2,
			PGDN = TUI_BUFFER_OFFSET + 3,

			DEL = TUI_BUFFER_OFFSET + 4,
			INS = TUI_BUFFER_OFFSET + 5,
			END = TUI_BUFFER_OFFSET + 6,
			HOME = TUI_BUFFER_OFFSET + 7,

			F1 = TUI_BUFFER_OFFSET + 8,
			F2 = TUI_BUFFER_OFFSET + 9,
			F3 = TUI_BUFFER_OFFSET + 10,
			F4 = TUI_BUFFER_OFFSET + 11,
			F5 = TUI_BUFFER_OFFSET + 12,
			F6 = TUI_BUFFER_OFFSET + 13,
			F7 = TUI_BUFFER_OFFSET + 14,
			F8 = TUI_BUFFER_OFFSET + 15,
			F9 = TUI_BUFFER_OFFSET + 16,
			F10 = TUI_BUFFER_OFFSET + 17,
			F11 = TUI_BUFFER_OFFSET + 18,
			F12 = TUI_BUFFER_OFFSET + 19,

			UP = TUI_BUFFER_OFFSET + 20,
			DOWN = TUI_BUFFER_OFFSET + 21,
			LEFT = TUI_BUFFER_OFFSET + 22,
			RIGHT = TUI_BUFFER_OFFSET + 23
		};

		struct terminal_info
		{
			terminal_info()
			{
				//parse terminfo
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

			std::vector<bool> buffer[2];
			std::vector<bool> second_buffer[2]; //buffer for "characters" that needs to return more than one time

			std::string string_buffer[2];
		public:
			bool operator[](int i) 
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
				for (;;)
				{
#ifdef  TUI_TARGET_SYSTEM_WINDOWS
					int pressed = _getch();

					if (pressed == 3)
					{
						CTRLC_handler();
					}

					if (pressed != 0 && pressed != 224)
					{
						if (pressed >= 0 && pressed < TUI_GETCH_RANGE)
						{
							buffer[1][pressed] = true;

							if (pressed >= 32)
							{
								string_buffer[1] += (char)pressed;
							}
						}
					}

					if (pressed == 0 || pressed == 224)
					{
						int second_getch = _getch();

						if (second_getch >= 0 && second_getch < TUI_GETCH_RANGE)
						{
							second_buffer[1][second_getch] = true;
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

		//return true if key was pressed in iteration before last clear
		inline bool isKeyPressed(int key)
		{
			return buffer[key];
		}

		//return string that consist of characters pressed in iteration before last clear
		inline std::string getInputAsString()
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
