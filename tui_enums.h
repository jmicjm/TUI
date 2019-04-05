#pragma once

#include <array>
#include <string>

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

	std::array<std::string, 16> linuxBG =
	{
		{
			"40",
			"44",
			"42",
			"46",
			"41",
			"45",
			"43",
			"47",
			"100",
			"104",
			"102",
			"106",
			"101",
			"105",
			"103",
			"107"
		}
	};

	std::array<std::string, 16> linuxFG =
	{
		{
			"30",
			"34",
			"32",
			"36",
			"31",
			"35",
			"33",
			"37",
			"30;1",
			"34;1",
			"32;1",
			"36;1",
			"31;1",
			"35;1",
			"33;1",
			"37;1"
		}
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