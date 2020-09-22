//this file contains declarations of output functions
#pragma once
#include "tui_surface.h"

namespace tui
{
	namespace output
	{
		void init();

		void restore();

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

		//update surface position relative to console size
		void updateSurfacePosition(surface&);

		void setFpslimit(unsigned int);

		bool isTimeToDisplay();

		void displayRgbColor(bool);
		bool isDisplayingRgbColor();

		void displayRgbiColor(bool);
		bool isDisplayingRgbiColor();

		void displayColor(bool);
		bool isDisplayingColor();
	}
}