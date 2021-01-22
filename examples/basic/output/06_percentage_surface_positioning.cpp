#include "tui_io.h"

int main()
{
	tui::init();

	tui::surface_size surface_size(tui::vec2i(4, 3));
	tui::surface_position surface_positon(tui::vec2i(0, 0), tui::vec2f(30, 20)); //30%,20% offset
	/*
	surface_positon.percentage_offset = {30,20}; //<-equivalent
	*/

	tui::surface surface;

	surface.setSizeInfo(surface_size);
	surface.setPositionInfo(surface_positon); //sets position info

	surface.fill({ 'A', tui::COLOR::RED });// fills with red 'A' charaters

	while (!tui::input::isKeyPressed('q'))
	{
		tui::output::clear();

		tui::output::draw(surface);

		tui::output::display();
	}

	return 0;
}