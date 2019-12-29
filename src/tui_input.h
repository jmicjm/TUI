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
			RIGHT = TUI_KEY_OFFSET + 21
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
