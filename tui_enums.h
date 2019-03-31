#pragma once

namespace tui
{
#define TRANSPARENT 0
#define BLANKSYMBOL ' '

	namespace DIRECTION
	{
		enum DIRECTION
		{
			HORIZONTAL = 0,
			VERTICAL
		};
		/*enum LENGHT
		{
			CONSTANT = 0,
			PERCENTAGE
		};*/
	}

		enum SIZE
		{
			CONSTANT = 0,
			PERCENTAGE = 1,
			PERCENTAGE_X,
			PERCENTAGE_Y
		};
		enum THICKNESS
		{
			THIN=0,
			MEDIUM,
			THICK
		};

		

	enum COLOR
	{
		BLACK = 0,
		BLUE = 1,
		GREEN,
		CYAN,
		RED,
		MAGENTA,
		BROWN,
		LIGHTGRAY,
		DARKGRAY,
		LIGHTBLUE,
		LIGHTGREEN,
		LIGHTCYAN,
		LIGHTRED,
		LIGHTMAGENTA,
		YELLOW,
		WHITE
	};
	namespace POSITION
	{
		namespace VERTICAL
		{
			enum VERTICAL
			{
				TOP = 0, CENTER = 50, BOTTOM = 100
			};
		}
		namespace HORIZONTAL
		{
			enum HORIZONTAL
			{
				LEFT = 0, CENTER = 50, RIGHT = 100
			};
		}
	}
}