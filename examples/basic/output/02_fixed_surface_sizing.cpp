#include "tui_io.h"

int main()
{
	tui::init(); //initializes io

	tui::surface_size surface_size(tui::vec2i(4, 3)); //fixed 4,3 size
	/*
		surface_size.fixed = tui::vec2i(4,3); //<-equivalent
	*/

	tui::surface surface; //creates surface with 1x1 size(default)

	surface.setSizeInfo(surface_size); //sets size info. Surface also can be constructed directly from surface_size

	surface[0][0] = 'A';
	surface[3][2] = 'B';

	while (!tui::input::isKeyPressed('q')) //checks for 'q' key press
	{
		tui::output::clear(); //clears buffer and resizes it to terminal size(if necessary)

		tui::output::draw(surface); //copies surface to buffer

		tui::output::display(); //displays buffer contents and swaps input buffers
	}

	return 0;
}