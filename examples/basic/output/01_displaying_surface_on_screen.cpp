#include "tui_io.h"

int main()
{
	tui::init(); //initializes io

	tui::surface surface; //creates surface with 1x1 size(default)

	surface[0][0] = 'A'; //assigns symbol 'A' to postion 0,0 on surface. Alternatively you can use surface.setSymbolAt('A', {0,0})

	while (!tui::input::isKeyPressed('q'))//checks for 'q' key press
	{
		tui::output::clear(); //clears buffer and resizes it to terminal size(if necessary)

		tui::output::draw(surface); //copies surface to buffer

		tui::output::display(); //displays buffer contents and swaps input buffers
	}

	return 0;
}