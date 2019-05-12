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
		vec2i m_fixed_size;
		vec2i m_percentage_size;

	public:
		surface_size() : surface_size({ 0,0 }, { 0,0 }) {}
		surface_size(vec2i size) : surface_size(size, { 0,0 }) {}
		surface_size(vec2i fixed, vec2i percentage)
		{
			setSize(fixed, percentage);
		}

		void setSize(vec2i fixed, vec2i percentage)
		{
			m_fixed_size = fixed;
			m_percentage_size = percentage;
		}
		vec2i getFixedSize() { return m_fixed_size; }
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
	struct color
	{
		private:

			int m_foreground;
			int m_background;
		public:
			color() : color(tui::COLOR::WHITE, tui::COLOR::BLACK) {}
			color(int foreground) : color(foreground, tui::COLOR::BLACK) {}
			color(int foreground, int background) { setColor(foreground, background); }
			void setColor(int foreground, int background)
			{
				m_background = background;
				m_foreground = foreground;
			}

			//swaps foreground color with background color
			void invert()
			{
				int foreground = m_foreground;
				m_foreground = m_background;
				m_background = foreground;
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
			bool operator==(color c)
			{
				if (c.m_background == m_background && c.m_foreground == m_foreground)
				{ return true; }
				else { return false; }
			}
			bool operator!=(color c)
			{
				if (c.m_background != m_background || c.m_foreground != m_foreground)
				{ return true; }
				else { return false; }
			}
			//operator int() { return getRGBIColor(); }
	};

	inline std::u32string Utf8ToUtf32(std::string str)
	{
#ifdef  TUI_TARGET_SYSTEM_WINDOWS
		static std::wstring_convert<std::codecvt_utf8<int32_t>, int32_t> converter;
		auto conv = converter.from_bytes(str);

		return std::u32string(reinterpret_cast<char32_t const*>(conv.data()), conv.length());
#endif

#ifdef  TUI_TARGET_SYSTEM_LINUX

		static std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
		return converter.from_bytes(str);
#endif
	}
	inline std::string Utf32ToUtf8(std::u32string str)
	{
#ifdef  TUI_TARGET_SYSTEM_WINDOWS
		static std::wstring_convert<std::codecvt_utf8<int32_t>, int32_t> converter;
		auto conv = reinterpret_cast<const int32_t*>(str.data());

		return converter.to_bytes(conv, conv + str.size()); 
#endif

#ifdef  TUI_TARGET_SYSTEM_LINUX

		static std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> converter;
		return converter.to_bytes(str);
#endif
	}

	inline std::string Char32ToUtf8(char32_t character)
	{
		std::u32string str;
		str.resize(1);
		str[0] = character;

		return Utf32ToUtf8(str);
	}

	inline size_t GetUtf8StrLength(std::string str)
	{
		return Utf8ToUtf32(str).size();
	}

	struct symbol
	{
		private:
			char32_t m_character = ' ';
			color m_color;
		public:
			symbol() : symbol(' ', color()) {}
			symbol(char32_t character) : symbol(character, color()) {}
			symbol(char32_t character, color color)
			{
				setSymbol(character);
				setColor(color);
			}

			void invert() { m_color.invert(); }

			void setSymbol(char32_t character) { m_character = character; }
			void setColor(color color) { m_color = color; }
			char32_t getSymbol() { return m_character; }
			color getColor() { return m_color; }
			//operator std::string() { return m_character; }

			//explicit operator char32_t() { return m_character; }

		/*	int getStrLen()
			{
				return GetUtf8StrLength(m_character);
			}*/

			bool operator==(symbol c)
			{
				if(m_character == c.m_character && m_color == c.m_color)
				{ return true; }
				else { return false; }
			}
			bool operator!=(symbol c)
			{
				if (m_character != c.m_character || m_color != c.m_color)
				{ return true; }
				else { return false; }
			}
	};

	

	struct console_string
	{
	private:
		std::vector<symbol> m_console_string;
		color m_selected_color;

		
	public:
		console_string(){}
		console_string(const console_string &str)
		{
			m_console_string.resize(str.size());

			for (int i = 0; i < m_console_string.size(); i++)
			{
				m_console_string[i].setSymbol(str[i].getSymbol());
				m_console_string[i].setColor(str[i].getColor());
			}
		}
		console_string(symbol Symbol)
		{
			m_console_string.resize(1);
			m_console_string[0] = Symbol;
		}

		console_string(const char32_t* str) : console_string(std::u32string(str)) {}
		console_string(std::u32string str) : console_string(str, color()) {}
		console_string(std::u32string str, color color)
		{
			m_console_string.resize(str.size());

			for (int i = 0; i < m_console_string.size(); i++)
			{
				m_console_string[i].setSymbol(str[i]);
				m_console_string[i].setColor(color);
			}
		}

		console_string(const char* str) : console_string(std::string(str)) {}
		console_string(std::string str) : console_string(str, color()) {}
		console_string(std::string str, color color) 
		{

			m_console_string.resize(GetUtf8StrLength(str));

			for (int i = 0; i < m_console_string.size(); i++)
			{
				std::u32string utf32_str = Utf8ToUtf32(str);

				std::u32string utf32_char;
				utf32_char.resize(1);
				utf32_char[0] = utf32_str[i];

				//std::string character = Utf32ToUtf8(utf32_char);


				m_console_string[i].setSymbol(utf32_char[0]);
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

		symbol &operator[] (int i)
		{
			return m_console_string[i];
		}
		symbol operator[] (int i) const
		{
			return m_console_string[i];
		}

		void operator<< (color color)
		{
			m_selected_color = color;
		}

		void operator<< (console_string string)
		{
			for (int i = 0; i < string.size(); i++)
			{
				m_console_string.push_back(symbol(string[i].getSymbol(), m_selected_color));
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

		

		void push_back(symbol Symbol)
		{
			m_console_string.push_back(Symbol);
		}

		int size() { return m_console_string.size(); }
		int size() const { return m_console_string.size(); }

		void invert()
		{
			for (int i = 0; i < m_console_string.size(); i++)
			{
				m_console_string[i].invert();
			}
		}

		void setColor(color Color)
		{
			for (int i = 0; i < m_console_string.size(); i++)
			{
				m_console_string[i].setColor(Color);
			}
		}

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


	inline bool isPunctuation(symbol Symbol)
	{
		if (Symbol == '.'
			|| Symbol == ','
			|| Symbol == ':'
			|| Symbol == ';'
			|| Symbol == '!'
			|| Symbol == '?')
		{
			return true;
		}
		else { return false; }
	}

	

}