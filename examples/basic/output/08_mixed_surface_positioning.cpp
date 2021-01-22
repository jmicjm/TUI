#include "tui_io.h"

int main()
{
	tui::init();

	tui::surface_position surface_positon({ -2, -2 }, { 0, 0 }, { tui::POSITION::END, tui::POSITION::END }); //located at the center of bottom side of screen + {-2,-2} offset
	/*
	surface_positon.offset = {-2,-2};
	surface_positon.relative = {tui::POSITION::END, tui::POSITION::END}; //<-equivalent
	*/

	tui::surface surface({ { 4,3 }, {0,0} });

	surface.setPositionInfo(surface_positon);

	surface.fill({ 'A', tui::COLOR::RED });

	while (!tui::input::isKeyPressed('q'))
	{
		tui::output::clear();

		tui::output::draw(surface);

		tui::output::display();
	}

	return 0;
}