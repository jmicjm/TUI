#include "tui_io.h"

int main()
{
	tui::init();

	tui::surface bigger({ {0,0},{25,25} });
	tui::surface smaller({ {5,3},{0,0} });

	smaller.fill({ U'\x2588', tui::COLOR::RED });

	bigger.setPositionInfo({ {0,0},{0,0},{tui::POSITION::CENTER, tui::POSITION::CENTER} });

	tui::anchor_position anchor_position = { tui::SIDE::BOTTOM, tui::POSITION::END, {0,1} }; // bottom side, located at the end, {0,1} offset
	/*
	possible tui::SIDE values:
	tui::SIDE::LEFT
	tui::SIDE::TOP
	tui::SIDE::RIGHT
	tui::SIDE::BOTTOM
	*/
	smaller.setAnchor(&bigger); // sets bigger as anchor
	smaller.setAnchorPositionInfo(anchor_position);

	while (!tui::input::isKeyPressed('q'))
	{
		tui::output::clear();

		tui::output::updateSurfaceSize(bigger);

		if (bigger.isResized())
		{
			bigger.fill({ U'\x2588', tui::COLOR::GREEN });
		}

		tui::output::draw(bigger);
		tui::output::draw(smaller);


		tui::output::display();
	}

	return 0;
}