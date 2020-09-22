#include "tui_io.h"

int main()
{
	tui::init();

	tui::surface_size surface_size(tui::vec2i(0, 0), tui::vec2f(30, 20)); //creates surface with size equal to 30%,20%
	/*
		surface_size.percentage = tui::vec2f(30,20); //<-equivalent
	*/

	tui::surface surface;

	surface.setSizeInfo(surface_size);

	while (!tui::input::isKeyPressed('q'))
	{
		tui::output::clear();

		tui::output::updateSurfaceSize(surface); // updates surface size with respect to current terminal size. Its also automatically called by draw() so if you dont care about size delayed by one frame you can ommit this

		if (surface.isResized()) //checks if surface was resized
		{
			tui::vec2i size = surface.getSize();// returns current surface size

			surface[0][0] = 'A';
			surface[size.x - 1][size.y - 1] = 'B';
		}

		tui::output::draw(surface);

		tui::output::display();
	}

	return 0;
}