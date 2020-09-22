#include "tui_io.h"

int main()
{
	tui::init();

	tui::surface_position surface_positon({ 0, 0 }, { 0, 0 }, { tui::POSITION::CENTER, tui::POSITION::END }); //located at the center of bottom side of screen
	/*
	surface_positon.relative = {tui::POSITION::CENTER, tui::POSITION::END}; //<-equivalent
	*/

	/*
	predefined values:
	tui::POSITION::BEGIN = 0
	tui::POSITION::CENTER = 50
	tui::POSITION::END = 100
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