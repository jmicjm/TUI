#pragma once

#include "tui_input.h"
#include "tui_output.h"

namespace tui
{
	inline void init()
	{
		output::init();
		input::init();
	}

	inline void restore()
	{
		output::restore();
		input::restore();
	}
}