#include "tui_io.h"

int main()
{
	tui::init();

	tui::surface surf = tui::symbol_string("press enter to invert this text");

	while (!tui::input::isKeyPressed('q'))
	{
		tui::output::clear();

		if (tui::input::isKeyPressed(tui::input::KEY::ENTER))
		{
			surf.invert();
		}

		tui::output::draw(surf);

		tui::output::display();
	}

	return 0;
}