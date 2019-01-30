#pragma once

#include "tui_enums.h"


namespace tui 
{
	struct vec2i
	{
		int x, y;
		vec2i() {}
		vec2i(int X, int Y)
		{
			x = X;
			y = Y;
		}
	};

	struct position
	{
		private:
			vec2i m_offset;
			vec2i m_relative;
		public:
			position() : position(vec2i(0, 0), vec2i(POSITION::HORIZONTAL::LEFT, POSITION::VERTICAL::TOP)) {}
			position(vec2i offset): position(offset, vec2i(POSITION::HORIZONTAL::LEFT, POSITION::VERTICAL::TOP)) {}
			position(vec2i offset, vec2i relative_point)
			{
				setOffset(offset);
				setRelativePoint(relative_point);
			}
			void setOffset(vec2i offset) { m_offset = offset; }
			void setRelativePoint(vec2i relative) { m_relative = relative; }
			vec2i getOffset() { return m_offset; }
			vec2i getRelativePoint() { return m_relative; }
	};

	struct console_color
	{
		private:
			int m_foreground = 7; //white
			int m_background = 0; //black
		public:
			console_color() {}
			console_color(int foreground, int background) { setColor(foreground, background); }
			void setColor(int foreground, int background)
			{
				m_background = background;
				m_foreground = foreground;
			}
			int getForegroundColor() { return m_foreground; }
			int getBackgoundColor() { return m_background; }
			operator int() { return 16 * m_background + m_foreground; }
	};

	struct console_char
	{
		private:
			char m_character = ' ';
			console_color m_color;
		public:
			console_char() : console_char(' ', console_color(tui::COLOR::WHITE, tui::COLOR::BLACK)) {}
			console_char(char character) : console_char(character, console_color(tui::COLOR::WHITE, tui::COLOR::BLACK)) {}
			console_char(char character, console_color color)
			{
				setChar(character);
				setColor(color);
			}

			void setChar(char character) { m_character = character; }
			void setColor(console_color color) { m_color = color; }
			char getChar() { return m_character; }
			console_color getColor() { return m_color; }
	};
}