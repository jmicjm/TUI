#pragma once
#include <string>
#include <vector>

#define TUI_KEY_OFFSET 0xF00 //could be any value above 255

namespace tui
{
	namespace input
	{
		enum KEY
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

		std::vector<short> getInput();

		//uninterpreted input
		std::string getRawInput();

		//alphanumerical + '/n'
		std::string getStringInput();

		//amount of key press
		int isKeyPressed(short key);

		//swap buffers
		void swap();

		//clear buffers
		void clear();




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
