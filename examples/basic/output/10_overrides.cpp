#include "tui_io.h"

int main()
{
    tui::init();

    tui::symbol_string text;
    text << tui::COLOR::RED << "example "
        << tui::color{ tui::COLOR::BLACK, tui::COLOR::RED } << "surface";

    tui::surface plain = text + " inserted without overrides";
    tui::surface ct_bg = text + " inserted with transparent background";
    ct_bg.setPositionInfo({ {0,1} });
    tui::surface ct_fg = text + " inserted with transparent foreground";
    ct_fg.setPositionInfo({ {0,2} });
    tui::surface c_fg = text + " inserted with overrided foreground color";
    c_fg.setPositionInfo({ {0,3} });
    tui::surface c_bg = text + " inserted with overrided background color";
    c_bg.setPositionInfo({ {0,4} });
    tui::surface c_bgfg = text + " inserted with overrided color";
    c_bgfg.setPositionInfo({ {0,5} });
    tui::surface c_fg_ct_bg = text + " inserted with overrided foreground color and transparent background";
    c_fg_ct_bg.setPositionInfo({ {0,6} });
    tui::surface c_bg_ct_fg = text + " inserted with overrided background color and transparent foreground";
    c_bg_ct_fg.setPositionInfo({ {0,7} });


    while (!tui::input::isKeyPressed('q'))
    {
        tui::output::clear(tui::COLOR::BLUE);

        tui::output::draw(plain);
        tui::output::draw(ct_bg,      tui::COLOR_TRANSPARENCY::BG);//transparent background
        tui::output::draw(ct_fg,      tui::COLOR_TRANSPARENCY::FG);//transparent foreground
        tui::output::draw(c_fg,       { tui::COLOR::GREEN, 1 });//overrided foreground color
        tui::output::draw(c_bg,       { tui::COLOR::GREEN, 0 });//overrided background color
        tui::output::draw(c_bgfg,     tui::color{ tui::COLOR::GREEN, tui::COLOR::YELLOW });//overrided color
        tui::output::draw(c_fg_ct_bg, { tui::COLOR::BROWN, 1 }, tui::COLOR_TRANSPARENCY::BG);//overrided foreground color + transparent background
        tui::output::draw(c_bg_ct_fg, { tui::COLOR::BROWN, 0 }, tui::COLOR_TRANSPARENCY::FG);//overrided background color + transparent foreground

        tui::output::display();
    }

    return 0;
}
