#pragma once

#include <array>
#include <string>

namespace tui
{
#define TRANSPARENT 0
#define BLANKSYMBOL ' '



		
		


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




	
}