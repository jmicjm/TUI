#pragma once
#include "tui_utils.h"
#include "tui_text_utils.h"
#include "tui_surface.h"
#include "tui_input.h"

#include <vector>
#include <string>
#include <iostream>

#ifdef TUI_TARGET_SYSTEM_WINDOWS
	#include <windows.h>
#endif

#ifdef TUI_TARGET_SYSTEM_LINUX
	#include <sys/ioctl.h>
	#include <unistd.h>
	#include <termios.h>
#endif


namespace tui
{
	namespace output
	{
		//clear buffer + update size
		void clear();

		//copy surface to buffer
		void draw(surface&);

		//display buffer content
		void display();

		//return console size
		vec2i getSize();

		bool isResized();

		//update surface size relative to console size
		void updateSurfaceSize(surface&);

		void setFpslimit(int);

		bool isTimeToDisplay();

		void displayRgbColor(bool);
		bool isDisplayingRgbColor();

		void displayRgbiColor(bool);
		bool isDisplayingRgbiColor();

		void displayColor(bool);
		bool isDisplayingColor();
	}
}