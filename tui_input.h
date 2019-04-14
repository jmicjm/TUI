#pragma once
#include <map>
#include <string>
#include <thread>
#include <chrono>



#ifdef  TUI_TARGET_SYSTEM_WINDOWS
	#include <conio.h>
#define TUI_GETCH_RANGE 256

#define TUI_BUFFER_OFFSET 0xF00 // could be any value above TUI_GETCH_RANGE




namespace tui
{
	struct value_string_pair
	{
		int value;
		std::string string;

		value_string_pair() {}
		value_string_pair(int val, std::string String)
		{
			value = val;
			string = String;
		}
	};

	namespace KEYBOARD
	{
		void CTRLC_handler()
		{

		}

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
			RIGHt = TUI_BUFFER_OFFSET + 77

		};

		
		
		struct keyboard_buffer
		{
		private:
			std::vector<bool> buffer[2];
			std::vector<bool> second_buffer[2];
			//std::vector<int> times_pressed[2];
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
					int pressed = _getch();

					if (pressed != 0 && pressed != 224)
					{
						buffer[1][pressed] = true;
						string_buffer[1] += (char)pressed;
					}

					if (pressed == 0 || pressed == 224)
					{
						int second_getch = _getch();

						second_buffer[1][second_getch] = true;
					}
				}
			}

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
		static keyboard_buffer buffer;

		inline bool isKeyPressed(int key)
		{
			return buffer[key];
		}

		inline std::string getInputAsString()
		{
			return buffer.getString();
		}

	}
}
#endif