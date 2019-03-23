#pragma once

#include "tui_enums.h"

#include <string>
#include <vector>


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
			int m_foreground;
			int m_background;
		public:
			console_color() { setColor(tui::COLOR::WHITE, tui::COLOR::BLACK); }
			console_color(int foreground) { setColor(foreground, tui::COLOR::BLACK); }
			console_color(int foreground, int background) { setColor(foreground, background); }
			void setColor(int foreground, int background)
			{
				m_background = background;
				m_foreground = foreground;
			}
			int getColor() { return 16 * m_background + m_foreground; }
			int getForegroundColor() { return m_foreground; }
			int getBackgoundColor() { return m_background; }
			operator int() { return getColor(); }
	};

	struct console_char
	{
		private:
			char m_character = ' ';
			console_color m_color;
		public:
			console_char() : console_char(' ', console_color()) {}
			console_char(char character) : console_char(character, console_color()) {}
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

	struct console_string
	{
		std::vector<console_char> m_console_string;
		int m_selected_color;


	};
}