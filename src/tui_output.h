//this file contains declarations of output functions
#pragma once
#include "tui_surface.h"

namespace tui
{
	namespace output
	{
		void init();

		void restore();

		//clears buffer + updates size
		void clear(rgb c = tui::COLOR::BLACK);

		//copies surface to buffer
		void draw(surface& surf, bool update = true);

		//displays buffer content
		void display();

		//returns console size
		vec2i getSize();

		bool isResized();

		//updates surface size relative to console size
		void updateSurfaceSize(surface& surf);

		//updates surface position relative to console size
		void updateSurfacePosition(surface& surf);

		void setFpslimit(unsigned int fps);

		bool isTimeToDisplay();

		void displayRgbColor(bool display);
		bool isDisplayingRgbColor();

		void displayRgbiColor(bool dsiplay);
		bool isDisplayingRgbiColor();

		void displayColor(bool display);
		bool isDisplayingColor();
	}
}