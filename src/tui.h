﻿#pragma once

#include "tui_output.h"
#include "tui_input.h"

#include "tui_rectangle.h"
#include "tui_box.h"
#include "tui_scroll.h"
#include "tui_text.h"
#include "tui_bar.h"
#include "tui_input_text.h"
#include "tui_button.h"
#include "tui_image.h"
#include "tui_chart.h"
#include "tui_animation.h"
#include "tui_radio_button.h"
#include "tui_line.h"
#include "tui_tabs.h"

#include "tui_group.h"
#include "tui_navigation_group.h"

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