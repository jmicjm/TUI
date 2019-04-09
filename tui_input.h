#pragma once
#include <map>
#include <string>
#include <thread>
#include <chrono>



#ifdef  TUI_TARGET_SYSTEM_WINDOWS
	#include <conio.h>
#define TUI_GETCH_RANGE 256

#define SPECIAL_OFFSET 0xF00




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

		
		
		struct keyboard_buffer
		{
		private:
			std::vector<bool> buffer[2];
			//std::vector<int> times_pressed[2];
			std::string string_buffer[2];
		public:
			bool operator[](int i) { return buffer[0][i]; }
			std::string getString() { return string_buffer[0]; }
			int size() { return buffer[0].size(); }

			keyboard_buffer()
			{
				buffer[0].resize(TUI_GETCH_RANGE);
				buffer[1].resize(TUI_GETCH_RANGE);
		
				std::thread keyboardBufferThread([this] {bufferThread(); });
				keyboardBufferThread.detach();
			}

			void bufferThread()
			{
				for (;;)
				{
					int pressed = _getch();

					buffer[1][pressed] = true;

					string_buffer[1] += (char)pressed;

					if (pressed == 0 || pressed == 224)
					{
						int second_getch = _getch();
					}
				}
			}

			void clear()
			{
				buffer[0] = buffer[1];
				string_buffer[0] = string_buffer[1];

				for (int i = 0; i < buffer[0].size(); i++)
				{
					buffer[1][i] = 0;
					string_buffer[1].clear();
				}
			}
		};
		static keyboard_buffer buffer;

		bool isKeyPressed(int key)
		{
			return buffer[key];
		}

		std::string getInputAsString()
		{
			return buffer.getString();
		}

	}
}
#endif