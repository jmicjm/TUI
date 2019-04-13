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