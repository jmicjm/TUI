#pragma once
#include <map>
#include <string>

#define SHIFT_OFFSET 0x900

namespace tui
{
	namespace KEYBOARD
	{

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
		DOWN  = VK_DOWN,
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
		
		EQUAL= VK_OEM_PLUS,
		MINUS = VK_OEM_MINUS,
		SLASH = VK_OEM_2, //?
		BACKSLASH = VK_OEM_5, //|
		TILDE = VK_OEM_3,
		LSQUAREBRACKET = VK_OEM_4, //{
		RSQUAREBRACKET = VK_OEM_6, //}
		SINGLEQUOTE = VK_OEM_7, //"


		CAPITAL_A = A + SHIFT_OFFSET,
		CAPITAL_B = B + SHIFT_OFFSET,

		};

		static std::map<int, std::string> key_string =
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
			{Z, "z"}
		};
	}



	bool isKeyPressed(int key)
	{
		if (key < SHIFT_OFFSET)
		{
			switch ((bool)(GetKeyState(key) & 0x8000))
			{
			case true:
				if (key != KEYBOARD::SHIFT && key != KEYBOARD::KEY::LSHIFT && key != KEYBOARD::KEY::RSHIFT)
				{
					switch ((bool)(GetKeyState(KEYBOARD::KEY::SHIFT) & 0x8000))
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
			switch ((bool)(GetKeyState(key - SHIFT_OFFSET) & 0x8000))
			{
			case true:
				switch ((bool)(GetKeyState(KEYBOARD::KEY::SHIFT) & 0x8000))
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


}