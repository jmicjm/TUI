#pragma once

#include "tui_enums.h"

#include <string>
#include <vector>
#include <array>
#include <chrono>
#include <thread>


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

		bool operator==(vec2i r)
		{
			if (x == r.x && y == r.y)
			{ return true; }
			else { return false; }
		}
		bool operator!=(vec2i r)
		{
			if (x != r.x || y != r.y)
			{ return true; }
			else { return false; }
		}
	};

	struct position
	{
		private:
			vec2i m_offset;
			vec2i m_percentage_offset;
			vec2i m_relative;
		public:
			position() : position(vec2i(0, 0), vec2i(0,0), vec2i(POSITION::HORIZONTAL::LEFT, POSITION::VERTICAL::TOP)) {}
			position(vec2i offset): position(offset, vec2i(0,0), vec2i(POSITION::HORIZONTAL::LEFT, POSITION::VERTICAL::TOP)) {}
			position(vec2i offset, vec2i percentage_offset) : position(offset, percentage_offset, vec2i(POSITION::HORIZONTAL::LEFT, POSITION::VERTICAL::TOP)) {}
			position(vec2i offset, vec2i percentage_offset, vec2i relative_point)
			{
				setOffset(offset);
				setRelativePoint(relative_point);
				setPercentageOffset(percentage_offset);
			}
			void setOffset(vec2i offset) { m_offset = offset; }
			void setPercentageOffset(vec2i percentage_offset) { m_percentage_offset = percentage_offset; }
			void setRelativePoint(vec2i relative) { m_relative = relative; }

			vec2i getOffset() { return m_offset; }
			vec2i getPercentageOffset() { return m_percentage_offset; }
			vec2i getRelativePoint() { return m_relative; }
	};

	struct RGB_color
	{
		int r, g, b;

		RGB_color() {}
		RGB_color(int R, int G, int B)
		{
			r = R;
			g = G;
			b = B;
		}
	};

	static const std::array<RGB_color, 16> c =
	{
		{
	   RGB_color(12,12,12),
	   RGB_color(12,12,12)
	   }
	};

	//4bit RGBI
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
			operator char() { return m_character; }
	};

	struct console_string
	{
	private:
		std::vector<console_char> m_console_string;
		int m_selected_color;

		
	public:
		console_string(){}
		console_string(const char* str) : console_string(std::string(str)) {}
		console_string(std::string string) : console_string(string, console_color()) {}
		console_string(std::string string, console_color color)
		{
			m_console_string.resize(string.size());

			for (int i = 0; i < m_console_string.size(); i++)
			{
				m_console_string[i].setChar(string[i]);
				m_console_string[i].setColor(color);
			}
		}

		void appendString(console_string string)
		{
			for (int i = 0; i < string.size(); i++)
			{
				m_console_string.push_back(string[i]);
			}
		}
		void assignString(console_string string)
		{
			m_console_string.resize(0);

			appendString(string);
		}

		console_char &operator[] (int i)
		{
			return m_console_string[i];
		}

		void operator<< (console_color color)
		{
			m_selected_color = color;
		}

		void operator<< (console_string string)
		{
			for (int i = 0; i < string.size(); i++)
			{
				m_console_string.push_back(console_char(string[i], m_selected_color));
			}
		}

		void operator= (console_string string)
		{
			assignString(string);
		}

		void operator= (const char* str)
		{
			std::string string(str);

			assignString(str);
		}

		void operator+=(console_string string)
		{
			appendString(string);
		}

		void operator+=(console_char con_char)
		{
			m_console_string.push_back(con_char);
		}

		std::string getStdString()
		{
			std::string string;
			string.resize(m_console_string.size());

			for (int i = 0; i < string.size(); i++)
			{
				string[i] = m_console_string[i];
			}

			return string;
		}

		operator std::string() { return getStdString(); }

		void push_back(console_char con_char)
		{
			m_console_string.push_back(con_char);
		}

		int size() { return m_console_string.size(); }

	};


	struct time_frame
	{
	private:
		std::chrono::milliseconds m_frame_time;
		std::chrono::steady_clock::time_point m_frame_start_point;
		std::chrono::steady_clock::time_point m_frame_end_point;

		void start()
		{
			m_frame_start_point = std::chrono::steady_clock::now();
		}
		void stop()
		{
			m_frame_end_point = std::chrono::steady_clock::now();
		}
		void restart()
		{
			stop();
			start();
		}

	public:
		time_frame(std::chrono::milliseconds frame_time)
		{
			setFrameTime(frame_time);
			start();
		}

		void setFrameTime(std::chrono::milliseconds frame_time)
		{
			m_frame_time = frame_time;
		}

		bool isEnd()
		{
			if (std::chrono::steady_clock::now() - m_frame_start_point >= m_frame_time)
			{
				restart();
				return true; 
			}
			else { return false; }
		}

		void sleepUntilEnd()
		{
			std::this_thread::sleep_until(m_frame_start_point + m_frame_time);
			restart();
		}

	};
}