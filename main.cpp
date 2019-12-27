#include "src/tui.h"


int main()
{
	tui::output::setFpslimit(30);

	tui::rectangle rect;
	rect.setSizeInfo({ { -2,-2 }, { 20,20 } });
	rect.setPositionInfo(tui::position({ 0, 0 }, { 0,0 }, { tui::POSITION::CENTER, tui::POSITION::CENTER }));

	tui::box mainBox({ {0,0}, { 100, 100 } });

	tui::box box1({ {20,0}, { 0, 100 } });
	box1.setPositionInfo(tui::position({ 0, 0 }, { 0,0 }, { tui::POSITION::END, tui::POSITION::BEGIN }));
	box1.setColor({ tui::COLOR::MAGENTA, tui::COLOR::BLACK });

	tui::console_string ipsum;
	ipsum << tui::color(tui::COLOR::CYAN);
	ipsum << u8"Lorem ipsum dolor sit amet, consectetur adipiscing elit. ";
	ipsum << tui::color(tui::COLOR::MAGENTA);
	ipsum << u8"Vestibulum feugiat egestas urna non euismod. ";
	ipsum << tui::color(tui::COLOR::BLACK, tui::COLOR::YELLOW);
	ipsum << u8"Maecenas ";
	ipsum << tui::color(tui::COLOR::MAGENTA);
	ipsum += u8"magna mauris, dictum non egestas eu, rhoncus sed sem. Cras egestas massa eget nulla cursus"
			 u8"venenatis. Nulla id ultricies arcu, id sollicitudin augue. Orci varius natoque penatibus e"
			 u8"t magnisdis parturient montes, nascetur ridiculus mus.";
	ipsum.push_back("x");

	tui::text text({ {18,0}, {0, 50} }, ipsum);
	text.setPositionInfo(tui::position({ -1, 1 }, { 0,0 }, { tui::POSITION::END, tui::POSITION::BEGIN }));
	text.activate();

	tui::chart chart;
	chart.setValues({ 8.8,8.5,-8.9,-4,-3,-2,-1,-0.5,7,4,-1,1,0.5,8,1,2,3,4 });
	chart.setSizeInfo({ {0,0},{33,33} });
	chart.setPositionInfo({ {1,1} });
	chart.activate();

	tui::animation anim;
	anim.setSizeInfo({ {2,2} });
	anim.setPositionInfo({ {-1,-1}, {0,0}, {tui::POSITION::END, tui::POSITION::END} });
	anim.setFPS(1);

	tui::image img[4];
	img[0].setImageColorOnly({ 1,2,3,4 },2);
	img[1].setImageColorOnly({ 4,1,2,3 }, 2);
	img[2].setImageColorOnly({ 3,4,1,2 }, 2);
	img[3].setImageColorOnly({ 2,3,4,1 }, 2);

	for (int i = 0; i < 4; i++) { anim.addFrame(img[i]); }


	for (;;)
	{
		tui::output::clear();

		tui::output::draw(rect);
		tui::output::draw(box1);
		tui::output::draw(mainBox);
		tui::output::draw(text);

		if (tui::input::isKeyPressed('a'))
		{
			chart.displayValueLabels(!chart.isDisplayingValueLabels());
		}
		if (tui::input::isKeyPressed('b'))
		{
			std::vector<float> v = chart.getValues();
			for (int i = 0; i < v.size(); i++) { v[i] = v[i] * -1; }
			chart.setValues(v);
		}
		if (tui::input::isKeyPressed('c'))
		{
			std::vector<float> v = chart.getValues();
			for (int i = 0; i < v.size(); i++) { v[i]++; }
			chart.setValues(v);
		}
		if (tui::input::isKeyPressed('d'))
		{
			std::vector<float> v = chart.getValues();
			for (int i = 0; i < v.size(); i++) { v[i]--; }
			chart.setValues(v);
		}

		tui::output::draw(chart);
		tui::output::draw(anim);

		tui::output::display();
	}
	
	return 0;
}