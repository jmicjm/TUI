//this file contains declarations of input functions and keys enums
#pragma once
#include <string>
#include <vector>

#define TUI_KEY_OFFSET 0xF00 //could be any value above 255

namespace tui
{
	namespace input
	{
		using key_t = short;

		enum KEY : key_t
		{
			ENTER = 13,
			BACKSPACE = 8,
			ESC = 27,
			TAB = 9,

			PGUP = TUI_KEY_OFFSET,
			PGDN = TUI_KEY_OFFSET + 1,
			DEL = TUI_KEY_OFFSET + 2,
			INS = TUI_KEY_OFFSET + 3,
			END = TUI_KEY_OFFSET + 4,
			HOME = TUI_KEY_OFFSET + 5,

			F1 = TUI_KEY_OFFSET + 6,
			F2 = TUI_KEY_OFFSET + 7,
			F3 = TUI_KEY_OFFSET + 8,
			F4 = TUI_KEY_OFFSET + 9,
			F5 = TUI_KEY_OFFSET + 10,
			F6 = TUI_KEY_OFFSET + 11,
			F7 = TUI_KEY_OFFSET + 12,
			F8 = TUI_KEY_OFFSET + 13,
			F9 = TUI_KEY_OFFSET + 14,
			F10 = TUI_KEY_OFFSET + 15,
			F11 = TUI_KEY_OFFSET + 16,
			F12 = TUI_KEY_OFFSET + 17,

			UP = TUI_KEY_OFFSET + 18,
			DOWN = TUI_KEY_OFFSET + 19,
			LEFT = TUI_KEY_OFFSET + 20,
			RIGHT = TUI_KEY_OFFSET + 21,

			//ALT KEYS
			ALT_PGUP = TUI_KEY_OFFSET + 22,
			ALT_PGDN = TUI_KEY_OFFSET + 23,
			ALT_DEL = TUI_KEY_OFFSET + 24,
			ALT_INS = TUI_KEY_OFFSET + 25,
			ALT_END = TUI_KEY_OFFSET + 26,
			ALT_HOME = TUI_KEY_OFFSET + 27,

			ALT_UP = TUI_KEY_OFFSET + 28,
			ALT_DOWN = TUI_KEY_OFFSET + 29,
			ALT_LEFT = TUI_KEY_OFFSET + 30,
			ALT_RIGHT = TUI_KEY_OFFSET + 31,
			/*
			ALT+F* omitted because linux environments tends to override them,
			on windows they dont work at all,
			so making reliable isKeySupported() would be painful
			*/

			//SHIFT KEYS
			//pgup,pgdn,home,end,ins,del - same as ALT+F*
			SHIFT_F1 = TUI_KEY_OFFSET + 32,
			SHIFT_F2 = TUI_KEY_OFFSET + 33,
			SHIFT_F3 = TUI_KEY_OFFSET + 34,
			SHIFT_F4 = TUI_KEY_OFFSET + 35,
			SHIFT_F5 = TUI_KEY_OFFSET + 36,
			SHIFT_F6 = TUI_KEY_OFFSET + 37,
			SHIFT_F7 = TUI_KEY_OFFSET + 38,
			SHIFT_F8 = TUI_KEY_OFFSET + 39,
			SHIFT_F9 = TUI_KEY_OFFSET + 40,
			SHIFT_F10 = TUI_KEY_OFFSET + 41,
			SHIFT_F11 = TUI_KEY_OFFSET + 42,
			SHIFT_F12 = TUI_KEY_OFFSET + 43,

			//not supported by windows console
			SHIFT_UP = TUI_KEY_OFFSET + 44,
			SHIFT_DOWN = TUI_KEY_OFFSET + 45,
			SHIFT_LEFT = TUI_KEY_OFFSET + 46,
			SHIFT_RIGHT = TUI_KEY_OFFSET + 47,

			//CTRL KEYS
			CTRL_PGUP = TUI_KEY_OFFSET + 48, //on windows same as F12
			CTRL_PGDN = TUI_KEY_OFFSET + 49,
			CTRL_DEL = TUI_KEY_OFFSET + 50,
			CTRL_INS = TUI_KEY_OFFSET + 51,
			CTRL_END = TUI_KEY_OFFSET + 52,
			CTRL_HOME = TUI_KEY_OFFSET + 53,

			CTRL_F1 = TUI_KEY_OFFSET + 54,
			CTRL_F2 = TUI_KEY_OFFSET + 55,
			CTRL_F3 = TUI_KEY_OFFSET + 56,
			CTRL_F4 = TUI_KEY_OFFSET + 57,
			CTRL_F5 = TUI_KEY_OFFSET + 58,
			CTRL_F6 = TUI_KEY_OFFSET + 59,
			CTRL_F7 = TUI_KEY_OFFSET + 60,
			CTRL_F8 = TUI_KEY_OFFSET + 61,
			CTRL_F9 = TUI_KEY_OFFSET + 62,
			CTRL_F10 = TUI_KEY_OFFSET + 63,
			CTRL_F11 = TUI_KEY_OFFSET + 64,
			CTRL_F12 = TUI_KEY_OFFSET + 65,

			CTRL_UP = TUI_KEY_OFFSET + 66,
			CTRL_DOWN = TUI_KEY_OFFSET + 67,
			CTRL_LEFT = TUI_KEY_OFFSET + 68,
			CTRL_RIGHT = TUI_KEY_OFFSET + 69,

			CTRL_A = 1,
			CTRL_B = 2,
			CTRL_C = 3,
			CTRL_D = 4,
			CTRL_E = 5,
			CTRL_F = 6,
			CTRL_G = 7,
			CTRL_H = 8,
			CTRL_I = 9,//TAB
			CTRL_J = 10,
			CTRL_K = 11,
			CTRL_L = 12,
			CTRL_M = 13,//ENTER
			CTRL_N = 14,
			CTRL_O = 15,
			CTRL_P = 16,
			CTRL_Q = 17,
			CTRL_R = 18,
			CTRL_S = 19,
			CTRL_T = 20,
			CTRL_U = 21,
			CTRL_V = 22,
			CTRL_W = 23,
			CTRL_X = 24,
			CTRL_Y = 25,
			CTRL_Z = 26
		};

		void init();

		void restore();

		std::vector<key_t> getInput();

		//uninterpreted input
		std::string getRawInput();

		//alphanumerical input
		std::string getStringInput();

		//amount of key press
		int isKeyPressed(key_t key);

		//amount of key press
		int isCodePointPressed(char32_t code_point);

		//swap buffers
		void swap();

		//clear buffers
		void clear();

		bool isKeySupported(key_t key);

		std::string getKeyName(key_t key, bool use_ctrl_name = false);


		/*NOTE ABOUT DOUBLE BUFFERING

		X -> clear()
		abcd -> key presses

						   IsKeyPressed() or get*Input() used here returns these characters
						    \/																	|
		|X|ab|cd|X|ab|cd|X|ab|cd|X|																|
				  /	    \																		|
				 |	     |<---------------------------------------------------------------------/

				without double buffering "cd" part will be lost		
		*/

	}
}
