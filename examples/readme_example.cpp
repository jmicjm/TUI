#include "tui_io.h"
#include "tui_box.h"
#include "tui_tabs.h"
#include "tui_text.h"
#include "tui_chart.h"
#include "tui_list.h"
#include "tui_input_text.h"
#include "tui_bar.h"
#include "tui_slider.h"
#include "tui_navigation_group.h"

#include <cmath>

int main()
{
	//BOX
	tui::box main_box({ {0,0}, {100,100} });
	main_box.setTitle("github.com/jmicjm/TUI");
	main_box.setTitlePosition(tui::POSITION::END);

	//TABS
	tui::tabs<tui::DIRECTION::HORIZONTAL> tabs({ 0,33 });
	tabs.setTabs({ "text", "chart", "list", "input_text", "bar", "slider" });
	tabs.key_next = tui::input::KEY::CTRL_RIGHT;
	tabs.key_prev = tui::input::KEY::CTRL_LEFT;
	tabs.activate();

	tui::symbol_string t_desc;
	t_desc += "next tab: ";
	t_desc << tui::COLOR::GREEN << tui::input::getKeyName(tabs.key_next);
	t_desc += ", prev tab: ";
	t_desc << tui::COLOR::GREEN << tui::input::getKeyName(tabs.key_prev);
	tui::surface tabs_keys(t_desc);
	tabs_keys.setPositionInfo({ {-1,0}, {0,0}, {tui::POSITION::END, tui::POSITION::END} });

	//TEXT
	tui::text text({ {0,0}, {50,50} });
	text.setPositionInfo({ {0,0}, {0,0}, {tui::POSITION::CENTER, tui::POSITION::CENTER} });
	tui::symbol_string str;
	for (char i = 33; i < 127; i++) { str.push_back(i); }
	str << tui::COLOR::GREEN << "\ncolored text " << tui::COLOR::YELLOW << u8"zażółć gęślą jaźń "
		<< tui::ATTRIBUTE::UNDERSCORE << tui::COLOR::CYAN << "underlined text";
	str += u8"\nｆｕｌｌｗｉｄｔｈ-> 全屏宽度 全角 전체 넓이";
	str += "\nLorem ipsum dolor sit amet, consectetur adipiscing elit. Mauris sed libero nisi. "
		"Etiam pellentesque ornare consequat. Sed congue nunc sit amet arcu tempor rhoncus. "
		"Nulla mattis erat justo. Nulla semper lorem quis massa laoreet venenatis. "
		"Mauris quis purus ut nulla finibus pharetra. Nulla non bibendum ipsum. "
		"Vivamus sem lorem, tincidunt sed efficitur fermentum, porttitor sit amet sem.";
	text.setText(str);

	//CHART
	tui::chart chart({ {0,0}, {50,50} });
	chart.setPositionInfo({ {0,0}, {0,0}, {tui::POSITION::CENTER, tui::POSITION::CENTER} });

	std::vector<tui::chart_data_unit> sin_data;
	for (int i = 0; i < 60; i++)
	{
		sin_data.push_back({ (float)std::sin(i / (3.14 * 1.5)), i % 15 == 0 ? tui::symbol(i / 15 + 48) + u8"Π" : "" });
	}
	chart.setData(sin_data);

	chart.displayValueLabels(true);
	chart.setValueLabelsPrecision(1);

	short chart_hide_dlabel = 'd';
	short chart_hide_vlabel = 'v';
	tui::symbol_string ck_desc;
	ck_desc += "show/hide data labels: ";
	ck_desc << tui::COLOR::GREEN << tui::input::getKeyName(chart_hide_dlabel);
	ck_desc += ", show/hide value labels: ";
	ck_desc << tui::COLOR::GREEN << tui::input::getKeyName(chart_hide_vlabel);
	tui::surface chart_keys(ck_desc);
	chart_keys.setAnchor(&chart);
	chart_keys.setAnchorPositionInfo({ tui::SIDE::BOTTOM, tui::POSITION::BEGIN, { 0,1 } });

	//LIST
	tui::list list({ {0,0}, {15,33} });
	list.setPositionInfo({ {0,0}, {0,0}, {tui::POSITION::CENTER, tui::POSITION::CENTER} });
	list.setEntries({
		{"a0", tui::CHECK_STATE::NOT_CHECKED}, {"a1", tui::CHECK_STATE::NOT_CHECKED}, {"a2"}, {"a3"}, {"a4"}, {"a5"},
		{
			"a6", tui::CHECK_STATE::NONCHECKABLE, nullptr, nullptr, nullptr,
			{
				{"b1"}, {"b2"}, {"b3"}, {"b4"}, {"b5"}, {"b6"},
				{
					"b7", tui::CHECK_STATE::NONCHECKABLE, nullptr, nullptr, nullptr,
					{
						{"x0", tui::CHECK_STATE::NOT_CHECKED}, {"x1"}, {"x2"}
					}
				}
			}
		},
		{"a7"}, {"a8"}, {"a9"}, {"a10"},
		{
			"a11", tui::CHECK_STATE::NOT_CHECKED, nullptr, nullptr, nullptr,
			{
				{"c1"}, {"c2"}
			}
		}
		});

	//INPUT_TEXT
	tui::input_text itxt({ {0,0}, {15,25} });
	itxt.setPositionInfo({ {0,0}, {0,0}, {tui::POSITION::CENTER, tui::POSITION::CENTER} });

	//BARS
	tui::bar<tui::DIRECTION::HORIZONTAL> bars[3];
	for (int i = 0; i < 3; i++)
	{
		bars[i].setSizeInfo({ 0, 20 });
		bars[i].displayPercentageLabel(true);
		bars[i].setMinValue(0);
		bars[i].setMaxValue(255);
		bars[i].setValue((i + 1) * 64);
		bars[i].setLabelsPrecision(0);
	}

	bars[0].setPositionInfo({ {0,-2},{0,0}, {tui::POSITION::CENTER, tui::POSITION::CENTER} });
	bars[1].setAnchor(&bars[0]);
	bars[1].setAnchorPositionInfo({ tui::SIDE::BOTTOM, tui::POSITION::CENTER, {0, 1} });
	bars[2].setAnchor(&bars[1]);
	bars[2].setAnchorPositionInfo({ tui::SIDE::BOTTOM, tui::POSITION::CENTER, {0, 1} });

	bars[0].setColor({ tui::COLOR::RED, tui::COLOR::DARKGRAY });
	bars[0].setPercentageColor(tui::COLOR::RED);
	bars[1].setColor({ tui::COLOR::GREEN, tui::COLOR::DARKGRAY });
	bars[1].setPercentageColor(tui::COLOR::GREEN);
	bars[2].setColor({ tui::COLOR::BLUE, tui::COLOR::DARKGRAY });
	bars[2].setPercentageColor(tui::COLOR::BLUE);

	//SLIDERS
	tui::slider<tui::DIRECTION::HORIZONTAL> sliders[3];
	for (int i = 0; i < 3; i++)
	{
		sliders[i].setSizeInfo({ 0, 20 });
		sliders[i].displayPercentageLabel(true);
		sliders[i].setMinValue(-100);
		sliders[i].setMaxValue(100);
		sliders[i].setValue(0);
		sliders[i].setLabelsPrecision(0);
	}

	sliders[0].setPositionInfo({ {0,-2},{0,0}, {tui::POSITION::CENTER, tui::POSITION::CENTER} });
	sliders[1].setAnchor(&sliders[0]);
	sliders[1].setAnchorPositionInfo({ tui::SIDE::BOTTOM, tui::POSITION::CENTER, {0, 1} });
	sliders[2].setAnchor(&sliders[1]);
	sliders[2].setAnchorPositionInfo({ tui::SIDE::BOTTOM, tui::POSITION::CENTER, {0, 1} });

	tui::navigation_group sliders_nav({ &sliders[0], &sliders[1], &sliders[2] });
	sliders_nav.key_next = tui::input::KEY::DOWN;
	sliders_nav.key_prev = tui::input::KEY::UP;

	//NAVIGATION GROUP
	tui::navigation_group tabs_nav({ &text, &chart, &list, &itxt, nullptr, &sliders_nav });
	tabs_nav.key_next = tui::input::KEY::CTRL_RIGHT;
	tabs_nav.key_prev = tui::input::KEY::CTRL_LEFT;
	tabs_nav.activate();

	tui::init();

	while (!tui::input::isKeyPressed(tui::input::KEY::ESC))
	{
		tui::output::clear();

		tui::output::draw(main_box);
		tui::output::draw(tabs);
		tui::output::draw(tabs_keys);
		tabs_nav.update();

		switch (tabs.getSelected())
		{
		case 0:
			tui::output::draw(text);
			break;
		case 1:
			tui::output::draw(chart);
			tui::output::draw(chart_keys);
			if (tui::input::isKeyPressed(chart_hide_dlabel))
			{
				chart.displayDataLabels(!chart.isDisplayingDataLabels());
			}
			if (tui::input::isKeyPressed(chart_hide_vlabel))
			{
				chart.displayValueLabels(!chart.isDisplayingValueLabels());
			}
			break;
		case 2:
			tui::output::draw(list);
			break;
		case 3:
			tui::output::draw(itxt);
			break;
		case 4:
			for (int i = 0; i < 3; i++)
			{
				tui::output::draw(bars[i]);
			}
			break;
		case 5:
			sliders_nav.update();
			for (int i = 0; i < 3; i++)
			{
				tui::output::draw(sliders[i]);
			}
			break;
		}


		tui::output::display();
	}

	return 0;
}