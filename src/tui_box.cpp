#include "tui_box.h"

namespace tui
{
	const box_appearance box_appearance::thin_line =   { U'\x2500', U'\x2500', U'\x2502', U'\x2502', U'\x250C', U'\x2510', U'\x2514', U'\x2518' };
	const box_appearance box_appearance::double_line = { U'\x2550', U'\x2550', U'\x2551', U'\x2551', U'\x2554', U'\x2557', U'\x255A', U'\x255D' };
	const box_appearance box_appearance::medium_line = { U'\x2580', U'\x2584', U'\x258C', U'\x2590', U'\x259B', U'\x259C', U'\x2599', U'\x259F' };
	const box_appearance box_appearance::thick_line =  { U'\x2588' };
}