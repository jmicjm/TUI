#pragma once

#include "tui_config.h"

#include "tui_enums.h"

#include <string>
#include <vector>
#include <array>
#include <chrono>
#include <thread>

#include <locale>
#include <codecvt>


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

	struct surface_size
	{
	private:
		vec2i m_integer_size;
		vec2i m_percentage_size;

	public:
		surface_size() : surface_size({ 0,0 }, { 0,0 }) {}
		surface_size(vec2i size) : surface_size(size, { 0,0 }) {}
		surface_size(vec2i size, vec2i percentage)
		{
			setSize(size, percentage);
		}

		void setSize(vec2i size, vec2i percentage)
		{
			m_integer_size = size;
			m_percentage_size = percentage;
		}
		vec2i getIntegerSize() { return m_integer_size; }
		vec2i getPercentagesize() { return m_percentage_size; }

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



	//4bit RGBI
	struct console_color
	{
		private:

			int m_foreground;
			int m_background;
		public:
			console_color() : console_color(tui::COLOR::WHITE, tui::COLOR::BLACK) {}
			console_color(int foreground) : console_color(foreground, tui::COLOR::BLACK) {}
			console_color(int foreground, int background) { setColor(foreground, background); }
			void setColor(int foreground, int background)
			{
				m_background = background;
				m_foreground = foreground;
			}
			int getRGBIColor() { return 16 * m_background + m_foreground; } //return windows console color
			int getForegroundColor() { return m_foreground; }
			int getBackgroundColor() { return m_background; }
			std::string getForegroundColorLinux() { return linuxFG[m_foreground]; }
			std::string getBackgroundColorLinux() { return linuxBG[m_background]; }
			std::string getEscapeCode()
			{
				std::string esc_c = "\033[";
				esc_c += getForegroundColorLinux();
				esc_c += ";";
				esc_c += getBackgroundColorLinux();
				esc_c += "m";

				return esc_c;
			}
			bool operator==(console_color c)
			{
				if (c.m_background == m_background && c.m_foreground == m_foreground)
				{ return true; }
				else { return false; }
			}
			bool operator!=(console_color c)
			{
				if (c.m_background != m_background || c.m_foreground != m_foreground)
				{ return true; }
				else { return false; }
			}
			//operator int() { return getRGBIColor(); }
	};

	struct console_char
	{
		private:
			wchar_t m_character = ' ';
			console_color m_color;
		public:
			console_char() : console_char(' ', console_color()) {}
			console_char(wchar_t character) : console_char(character, console_color()) {}
			console_char(wchar_t character, console_color color)
			{
				setChar(character);
				setColor(color);
			}

			void setChar(wchar_t character) { m_character = character; }
			void setColor(console_color color) { m_color = color; }
			wchar_t getChar() { return m_character; }
			console_color getColor() { return m_color; }
			operator wchar_t() { return m_character; }

			bool operator==(console_char c)
			{
				if(m_character == c.m_character && m_color == c.m_color)
				{ return true; }
				else { return false; }
			}
			bool operator!=(console_char c)
			{
				if (m_character != c.m_character || m_color != c.m_color)
				{ return true; }
				else { return false; }
			}
	};

	struct console_string
	{
	private:
		std::vector<console_char> m_console_string;
		console_color m_selected_color;
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		
	public:
		console_string(){}
		console_string(const console_string &str)
		{
			m_console_string.resize(str.size());

			for (int i = 0; i < m_console_string.size(); i++)
			{
				m_console_string[i].setChar(str[i]);
				m_console_string[i].setColor(str[i].getColor());
			}
		}
		console_string(console_char ch)
		{
			m_console_string.resize(1);
			m_console_string[0] = ch;
		}

		console_string(char ch)
		{
			m_console_string.resize(1);
			m_console_string[0] = ch;
		}
		console_string(const char* str) : console_string(std::string(str)) {}
		console_string(std::string str) : console_string(str, console_color()) {}
		console_string(std::string str, console_color color) 
		{
			std::wstring wstr = converter.from_bytes(str);

			m_console_string.resize(wstr.size());

			for (int i = 0; i < m_console_string.size(); i++)
			{
				m_console_string[i].setChar(wstr[i]);
				m_console_string[i].setColor(color);
			}
		}
		
		console_string(const wchar_t* str) : console_string(std::wstring(str)) {}
		console_string(std::wstring wstr) : console_string(wstr, console_color()) {}
		console_string(std::wstring wstr, console_color color)
		{
			m_console_string.resize(wstr.size());

			for (int i = 0; i < m_console_string.size(); i++)
			{
				m_console_string[i].setChar(wstr[i]);
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
		console_char operator[] (int i) const
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

		void operator+=(console_string string)
		{
			appendString(string);
		}

		std::wstring getStdString()
		{
			std::wstring string;
			string.resize(m_console_string.size());

			for (int i = 0; i < string.size(); i++)
			{
				string[i] = m_console_string[i];
			}

			return string;
		}

		operator std::wstring() { return getStdString(); }

		void push_back(console_char con_char)
		{
			m_console_string.push_back(con_char);
		}

		int size() { return m_console_string.size(); }
		int size() const { return m_console_string.size(); }

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

		bool isEnd(bool restart_clock)
		{
			if (std::chrono::steady_clock::now() - m_frame_start_point >= m_frame_time)
			{
				if (restart_clock == true)
				{
					restart();
				}
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


	inline bool isPunctuation(wchar_t symbol)
	{
		if (symbol == '.'
			|| symbol == ','
			|| symbol == ':'
			|| symbol == ';'
			|| symbol == '!'
			|| symbol == '?')
		{
			return true;
		}
		else { return false; }
	}

	inline std::u32string Utf8ToUtf32(std::string str)
	{
#ifdef  TUI_TARGET_SYSTEM_WINDOWS
		std::wstring_convert<std::codecvt_utf8<int32_t>, int32_t> converter;
		auto conv = converter.from_bytes(str);

		return std::u32string(reinterpret_cast<char32_t const*>(conv.data()), conv.length());
#endif

#ifdef  TUI_TARGET_SYSTEM_LINUX

		std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
		return converter.from_bytes(str);
#endif
	}

	inline size_t GetUtf8StrLength(std::string str)
	{
		return Utf8ToUtf32(str).size();
	}

}