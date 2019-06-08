#pragma once
#include "tui_enums.h"

#include <locale>
#include <codecvt>
#include <string>
#include <vector>


#include <iostream>

namespace tui
{
	std::u32string Utf8ToUtf32(std::string str);

	std::string Utf32ToUtf8(std::u32string str);

	std::string Char32ToUtf8(char32_t character);

	size_t GetUtf8StrLength(std::string str);

	int GetGraphemeType(char32_t grapheme);

	bool IsBreakBetween(char32_t l, char32_t r);



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
			{
				return true;
			}
			else { return false; }
		}
		bool operator!=(color c)
		{
			if (c.m_background != m_background || c.m_foreground != m_foreground)
			{
				return true;
			}
			else { return false; }
		}
		//operator int() { return getRGBIColor(); }
	};

	struct symbol
	{
	private:
		std::u32string m_grapheme_cluster = U" ";
		color m_color;
	public:
		symbol() : symbol(U" ", color()) {}
		symbol(const char32_t* Symbol) : symbol(std::u32string(Symbol), color()) {}
		symbol(const char32_t* Symbol, color color) : symbol(std::u32string(Symbol), color) {}
		symbol(std::u32string Symbol) : symbol(Symbol, color()) {}
		symbol(std::u32string Symbol, color color)
		{
			setSymbol(Symbol);
			setColor(color);
		}
		symbol(char32_t character) : symbol(character, color()) {}
		symbol(char32_t character, color color)
		{
			m_grapheme_cluster.resize(1);
			m_grapheme_cluster[0] = character;
			setColor(color);
		}

		void setSymbol(std::u32string symbol) 
		{
			std::u32string temp;
			temp += symbol[0];

			for (int i = 0; i < symbol.size()-1; i++)
			{	
				if (!IsBreakBetween(symbol[i], symbol[i + 1]))
				{
					temp += symbol[i + 1];
				}
				else { break; }
			}
			m_grapheme_cluster = temp;
		}

		void invert() { m_color.invert(); }


		char32_t getFirstChar() { return m_grapheme_cluster[0]; }
		void setColor(color color) { m_color = color; }
		std::u32string getSymbol() { return m_grapheme_cluster; }
		color getColor() { return m_color; }

		bool operator==(symbol c)
		{
			if (m_grapheme_cluster == c.m_grapheme_cluster && m_color == c.m_color)
			{
				return true;
			}
			else { return false; }
		}
		bool operator!=(symbol c)
		{
			if (m_grapheme_cluster != c.m_grapheme_cluster || m_color != c.m_color)
			{
				return true;
			}
			else { return false; }
		}
	};

	struct console_string
	{
	private:
		std::vector<symbol> m_console_string;
		color m_selected_color;

	public:
		console_string() {}
		console_string(symbol Symbol)
		{
			m_console_string.resize(1);
			m_console_string[0] = Symbol;
		}
		console_string(const char* str) : console_string(Utf8ToUtf32(str)) {}
		console_string(const char32_t* str) : console_string(std::u32string(str)) {}
		console_string(std::string str) : console_string(Utf8ToUtf32(str), color()) {}
		console_string(std::u32string str) : console_string(str, color()) {}
		console_string(std::string str, color color) : console_string(Utf8ToUtf32(str), color) {}	
		console_string(std::u32string str, color color)
		{
			std::vector<symbol> temp_vec;

			if (str.size() > 0)
			{
				int i = 0;
				while (i < str.size())
				{
					std::u32string temp;
					temp += str[i];

					for (; (i<str.size()-1 && !IsBreakBetween(str[i], str[i + 1])); i++)
					{
						temp += str[i + 1];
					}			
					temp_vec.push_back(symbol(temp, color));
					i++;
				}
			}
			m_console_string = temp_vec;
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

		symbol& operator[] (int i)
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


	inline int GetGraphemeType(char32_t grapheme)
	{
		struct grapheme_range_info { int r_s, r_e, r_t; };

		static const std::vector<grapheme_range_info> ranges =
		{
			#include "grapheme_ranges.h"
		};

		int s = -1;
		int e = ranges.size();

		while (s != e - 1)
		{
			if (ranges[(s + e) / 2].r_s > grapheme)
			{
				e = (s + e) / 2;
			}
			else if (ranges[(s + e) / 2].r_e < grapheme)
			{
				s = (s + e) / 2;
			}
			else
			{
				return ranges[(s + e) / 2].r_t;
			}
		}
		return GRAPHEME_TYPE::OTHER;
	}

	inline bool IsBreakBetween(char32_t l, char32_t r)
	{
		int l_t = GetGraphemeType(l);
		int r_t = GetGraphemeType(r);

		/*
		http://www.unicode.org/reports/tr29/
		This is not complete implementation of above(Regional_indicator is ommited)
		*/

		//GB3
		if (l_t == GRAPHEME_TYPE::CR
			&&
			r_t == GRAPHEME_TYPE::LF)
		{
			return false;
		}
		//GB4
		if (   l_t == GRAPHEME_TYPE::CONTROL
			|| l_t == GRAPHEME_TYPE::CR
			|| l_t == GRAPHEME_TYPE::LF)
		{
			return true;
		}
		//GB5
		if (   r_t == GRAPHEME_TYPE::CONTROL
			|| r_t == GRAPHEME_TYPE::CR
			|| r_t == GRAPHEME_TYPE::LF)
		{
			return true;
		}
		//GB6
		if (       l_t == GRAPHEME_TYPE::L
			&&
			    (  r_t == GRAPHEME_TYPE::L
				|| r_t == GRAPHEME_TYPE::V
				|| r_t == GRAPHEME_TYPE::LV
				|| r_t == GRAPHEME_TYPE::LVT)
			)
		{
			return false;
		}
		//GB7
		if (    (  l_t == GRAPHEME_TYPE::LV
			    || l_t == GRAPHEME_TYPE::V)
			&&
			    (  r_t == GRAPHEME_TYPE::V
				|| r_t == GRAPHEME_TYPE::T)
			)
		{
			return false;
		}
		//GB8
		if (    (  l_t == GRAPHEME_TYPE::LVT
				|| l_t == GRAPHEME_TYPE::T)
			&&
			   	   r_t == GRAPHEME_TYPE::T
			)
		{
			return false;
		}
		//GB9
		if (   r_t == GRAPHEME_TYPE::EXTEND
			|| r_t == GRAPHEME_TYPE::ZWJ)
		{
			return false;
		}
		//GB9a
		if (r_t == GRAPHEME_TYPE::SPACINGMARK)
		{
			return false;
		}
		//GB9b
		if (l_t == GRAPHEME_TYPE::PREPEND)
		{
			return false;
		}
		//GB999
		return true;
	}


	inline bool IsPunctuation(symbol Symbol)
	{
		if (   Symbol == U"."
			|| Symbol == U","
			|| Symbol == U":"
			|| Symbol == U";"
			|| Symbol == U"!"
			|| Symbol == U"?")
		{
			return true;
		}
		else { return false; }
	}
}
