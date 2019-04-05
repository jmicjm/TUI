#pragma once
#include <map>
#include <string>
#include <thread>
#include <chrono>

#ifdef  TUI_TARGET_SYSTEM_WINDOWS

#define SHIFT_OFFSET 0xF00

#define TUI_UNBUFFERED_INPUT 0
#define TUI_BUFFERED_INPUT 1


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
		bool isKeyPressed(int key, int buffer_type);

		enum KEY
		{
			NUMBER0 = 0x30,
			NUMBER1 = 0x31,
			NUMBER2 = 0x32,
			NUMBER3 = 0x33,
			NUMBER4 = 0x34,
			NUMBER5 = 0x35,
			NUMBER6 = 0x36,
			NUMBER7 = 0x37,
			NUMBER8 = 0x38,
			NUMBER9 = 0x39,

			A = 0x41,
			B = 0x42,
			C = 0x43,
			D = 0x44,
			E = 0x45,
			F = 0x46,
			G = 0x47,
			H = 0x48,
			I = 0x49,
			J = 0x4A,
			K = 0x4B,
			L = 0x4C,
			M = 0x4D,
			N = 0x4E,
			O = 0x4F,
			P = 0x50,
			Q = 0x51,
			R = 0x52,
			S = 0x53,
			T = 0x54,
			U = 0x55,
			V = 0x56,
			W = 0x57,
			X = 0x58,
			Y = 0x59,
			Z = 0x5A,

			BACKSPACE = VK_BACK,
			TAB = VK_TAB,
			ENTER = VK_RETURN,
			SHIFT = VK_SHIFT,
			CTRL = VK_CONTROL,
			ALT = VK_MENU,
			ESC = VK_ESCAPE,
			SPACE = VK_SPACE,
			PGUP = VK_PRIOR,
			PGDN = VK_NEXT,
			END = VK_END,
			HOME = VK_HOME,
			INS = VK_INSERT,
			DEL = VK_DELETE,

			LSHIFT = VK_LSHIFT,
			RSHIFT = VK_RSHIFT,
			LCTRL = VK_LCONTROL,
			RCTRL = VK_RCONTROL,
			LALT = VK_LMENU,
			RALT = VK_RMENU,

			UP = VK_UP,
			DOWN = VK_DOWN,
			LEFT = VK_LEFT,
			RIGHT = VK_RIGHT,

			SEPARATOR = VK_SEPARATOR,
			DECIMAL = VK_DECIMAL,

			F1 = 0x70,
			F2 = 0x71,
			F3 = 0x72,
			F4 = 0x73,
			F5 = 0x74,
			F6 = 0x75,
			F7 = 0x76,
			F8 = 0x77,
			F9 = 0x78,
			F10 = 0x79,
			F11 = 0x7A,
			F12 = 0x7B,

			SEMICOLON = VK_OEM_1, //:
			COMMA = VK_OEM_COMMA,
			DOT = VK_OEM_PERIOD,

			EQUAL = VK_OEM_PLUS,
			MINUS = VK_OEM_MINUS,
			SLASH = VK_OEM_2, //?
			BACKSLASH = VK_OEM_5, //|
			TILDE = VK_OEM_3,
			LSQUAREBRACKET = VK_OEM_4, //{
			RSQUAREBRACKET = VK_OEM_6, //}
			SINGLEQUOTE = VK_OEM_7, //"


			CAPITAL_A = A + SHIFT_OFFSET,
			CAPITAL_B = B + SHIFT_OFFSET,
			CAPITAL_C = C + SHIFT_OFFSET,
			CAPITAL_D = D + SHIFT_OFFSET,
			CAPITAL_E = E + SHIFT_OFFSET,
			CAPITAL_F = F + SHIFT_OFFSET,
			CAPITAL_G = G + SHIFT_OFFSET,
			CAPITAL_H = H + SHIFT_OFFSET,
			CAPITAL_I = I + SHIFT_OFFSET,
			CAPITAL_J = J + SHIFT_OFFSET,
			CAPITAL_K = K + SHIFT_OFFSET,
			CAPITAL_L = L + SHIFT_OFFSET,
			CAPITAL_M = M + SHIFT_OFFSET,
			CAPITAL_N = N + SHIFT_OFFSET,
			CAPITAL_O = O + SHIFT_OFFSET,
			CAPITAL_P = P + SHIFT_OFFSET,
			CAPITAL_Q = Q + SHIFT_OFFSET,
			CAPITAL_R = R + SHIFT_OFFSET,
			CAPITAL_S = S + SHIFT_OFFSET,
			CAPITAL_T = T + SHIFT_OFFSET,
			CAPITAL_U = U + SHIFT_OFFSET,
			CAPITAL_V = V + SHIFT_OFFSET,
			CAPITAL_W = W + SHIFT_OFFSET,
			CAPITAL_X = X + SHIFT_OFFSET,
			CAPITAL_Y = Y + SHIFT_OFFSET,
			CAPITAL_Z = Z + SHIFT_OFFSET,


			CAPSLK = VK_CAPITAL
		};

		//change to map
		static const std::vector<value_string_pair> key_string =
		{
			{NUMBER0, "0"},
			{NUMBER1, "1"},
			{NUMBER2, "2"},
			{NUMBER3, "3"},
			{NUMBER4, "4"},
			{NUMBER5, "5"},
			{NUMBER6, "6"},
			{NUMBER7, "7"},
			{NUMBER8, "8"},
			{NUMBER9, "9"},

			{A, "a"},
			{B, "b"},
			{C, "c"},
			{D, "d"},
			{E, "e"},
			{F, "f"},
			{G, "g"},
			{H, "h"},
			{I, "i"},
			{J, "j"},
			{K, "k"},
			{L, "l"},
			{M, "m"},
			{N, "n"},
			{O, "o"},
			{P, "p"},
			{Q, "q"},
			{R, "r"},
			{S, "s"},
			{T, "t"},
			{U, "u"},
			{V, "v"},
			{W, "w"},
			{X, "x"},
			{Y, "y"},
			{Z, "z"},

			{CAPITAL_A, "A"},
			{CAPITAL_B, "B"},
			{CAPITAL_C, "C"},
			{CAPITAL_D, "D"},
			{CAPITAL_E, "E"},
			{CAPITAL_F, "F"},
			{CAPITAL_G, "G"},
			{CAPITAL_H, "H"},
			{CAPITAL_I, "I"},
			{CAPITAL_J, "J"},
			{CAPITAL_K, "K"},
			{CAPITAL_L, "L"},
			{CAPITAL_M, "M"},
			{CAPITAL_N, "N"},
			{CAPITAL_O, "O"},
			{CAPITAL_P, "P"},
			{CAPITAL_Q, "Q"},
			{CAPITAL_R, "R"},
			{CAPITAL_S, "S"},
			{CAPITAL_T, "T"},
			{CAPITAL_U, "U"},
			{CAPITAL_V, "V"},
			{CAPITAL_W, "W"},
			{CAPITAL_X, "X"},
			{CAPITAL_Y, "Y"},
			{CAPITAL_Z, "Z"},

			{SEMICOLON, ";"},
			{COMMA, ","},
			{DOT, "."},


			{EQUAL, "="},
			{MINUS, "-"},
			{SLASH, "/"},
			{BACKSLASH, "\\"},
			{TILDE, "~"},
			{LSQUAREBRACKET, "["},
			{RSQUAREBRACKET, "]"},
			{SINGLEQUOTE, "'"},

			{UP, ""},
			{DOWN, ""},
			{LEFT, ""},
			{RIGHT,""},

			{CTRL, "CTRL"},
			{LCTRL, ""},
			{RCTRL,""}
		};

		
		struct keyboard_buffer
		{
		private:
			std::vector<bool> buffer[2];
			std::string string_buffer[2];
		public:
			bool operator[](int i) { return buffer[0][i]; }
			std::string getString() { return string_buffer[0]; }
			int size() { return buffer[0].size(); }

			keyboard_buffer()
			{
				buffer[0].resize(key_string.size());
				buffer[1].resize(key_string.size());
		
				std::thread keyboardBufferThread([this] {bufferThread(); });
				keyboardBufferThread.detach();
			}

			void bufferThread()
			{
				std::vector<std::chrono::steady_clock::time_point> last_key_press(buffer[1].size(), std::chrono::steady_clock::now());
				std::vector<int> is_key_hold(buffer[1].size(), 0);

				auto indexToString = [](int i)
				{
					return key_string[i].string;	
				};

				for (;;)
				{
					for (int i = 0; i < key_string.size(); i++)
					{
						if (isKeyPressed(key_string[i].value, TUI_UNBUFFERED_INPUT))
						{
							buffer[1][i] = true;	

							if (   is_key_hold[i] == 0	
								||(is_key_hold[i] == 1 && (std::chrono::steady_clock::now() - last_key_press[i] > std::chrono::milliseconds(500)))
								|| (is_key_hold[i] > 1 && (std::chrono::steady_clock::now() - last_key_press[i] > std::chrono::milliseconds(25)))
							   )
							{
								string_buffer[1] += indexToString(i);

								if (is_key_hold[i] <= 1) { is_key_hold[i]++; }
								last_key_press[i] = std::chrono::steady_clock::now();
							}
						}
						else
						{
							is_key_hold[i] = false;
						}
					}
					std::this_thread::sleep_for(std::chrono::milliseconds(20));
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



		bool isCapsLockEnabled(){ return (bool)GetKeyState(KEY::CAPSLK); }

		bool isKeyPressedUnbuffered(int key)
		{
			auto getKeyState = [](int key)
			{
					return (bool)(GetKeyState(key) & 0x8000);
			};

			if (key < SHIFT_OFFSET)
			{
				switch (getKeyState(key))
				{
				case true:
					if (key != KEY::SHIFT && key != KEY::LSHIFT && key != KEY::RSHIFT)
					{
						switch (getKeyState(KEY::SHIFT) ^ isCapsLockEnabled())
						{
						case true:
							return false;
						case false:
							return true;
						}
					}
					else
					{
						return true;
					}
				case false:
					return false;
				}
			}
			else
			{
				switch (getKeyState(key - SHIFT_OFFSET))
				{
				case true:
					switch (getKeyState(KEY::SHIFT) ^ isCapsLockEnabled())
					{
					case true:
						return true;
					case false:
						return false;
					}
				case false:
					return false;
				}
			}
		}

		bool isKeyPressedBuffered(int key)
		{
			for (int i = 0; i < buffer.size(); i++)
			{
				if (key_string[i].value == key)		
				{
					if (buffer[i] == true)
					{
						return true;
					}
				}
			}
			return false;
		}

		bool isKeyPressed(int key, int buffer_type)
		{
			switch (buffer_type)
			{
			case TUI_UNBUFFERED_INPUT:
				return isKeyPressedUnbuffered(key);
			case TUI_BUFFERED_INPUT:
				return isKeyPressedBuffered(key);
			}
		}


		std::string getInputAsStringUnbuffered()
		{
			std::string string;
			for (int i = 0; i < key_string.size(); i++)
			{
				if (isKeyPressed(key_string[i].value, TUI_UNBUFFERED_INPUT))
				{
					string += key_string[i].string;
				}
			}
			return string;
		}

		std::string getInputAsStringBuffered()
		{
			return buffer.getString();
		}

		std::string getInputAsString(int buffer_type)
		{
			switch (buffer_type)
			{
			case TUI_UNBUFFERED_INPUT:
				return getInputAsStringUnbuffered();
			case TUI_BUFFERED_INPUT:
				return getInputAsStringBuffered();
			}
		}

	}
}
#endif