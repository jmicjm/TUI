#include "tui_io.h"

int main()
{
	tui::init();

	tui::surface surf;
	surf.setPositionInfo({ {0,0},{0,0},{tui::POSITION::CENTER, tui::POSITION::CENTER} });

	while (!tui::input::isKeyPressed('q'))
	{
		tui::output::clear();

		/*
		tui::input::getInput() returns std::vector<short> that contains characters
		and special keys(like arrows) entered since last buffers swap(tui::output::display() or tui::input::swap())
		*/
		if (tui::input::getInput().size() > 0)
		{
			//string could be assigned to surface
			surf = tui::input::getKeyName(tui::input::getInput()[0]); // tui::input::getKeyName(short key) returns string containing key name
		}

		tui::output::draw(surf);

		tui::output::display();
	}

	return 0;
}