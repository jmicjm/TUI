#pragma once

namespace tui
{
	struct rgb
	{
		uint8_t r, g, b;

		rgb() : rgb(0, 0, 0) {}
		rgb(uint8_t R, uint8_t G, uint8_t B) : r(R), g(G), b(B) {}

		bool operator==(rgb rhs)
		{
			return r == rhs.r && g == rhs.g && b == rhs.b;
		}
		bool operator!=(rgb rhs) { return !operator==(rhs); }
	};

	namespace COLOR
	{
		const rgb BLACK = { 12,12,12 };
		const rgb BLUE = { 0,55,218 };
		const rgb GREEN = { 19,161,14 };
		const rgb CYAN = { 58,150,221 };
		const rgb RED = { 197,15,31 };
		const rgb MAGENTA = { 136,23,152 };
		const rgb BROWN = { 193,156,0 };
		const rgb LIGHTGRAY = { 204,204,204 };
		const rgb DARKGRAY = { 118,118,118 };
		const rgb LIGHTBLUE = { 59,120,255 };
		const rgb LIGHTGREEN = { 22,198,12 };
		const rgb LIGHTCYAN = { 97,214,214 };
		const rgb LIGHTRED = { 231,72,86 };
		const rgb LIGHTMAGENTA = { 180,0,158 };
		const rgb YELLOW = { 249,241,165 };
		const rgb WHITE = { 242,242,242 };
	}

	struct color
	{
		rgb foreground;
		rgb background;

		color() : color(tui::COLOR::WHITE, tui::COLOR::BLACK) {}
		color(rgb foreground) : color(foreground, tui::COLOR::BLACK) {}
		color(rgb Foreground, rgb Background) : foreground(Foreground), background(Background) {}

		//swaps foreground color with background color
		void invert()
		{
			rgb fg = foreground;
			foreground = background;
			background = fg;
		}

		bool operator==(color c) const
		{
			return c.background == background && c.foreground == foreground;
		}
		bool operator!=(color c) const { return !operator==(c); }
	};
}
