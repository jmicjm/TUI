#pragma once
#include "tui_enums.h"

#include <locale>
#include <codecvt>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>


#include <iostream>

namespace tui
{
	std::u32string Utf8ToUtf32(const std::string& utf8_str);

	std::string Utf32ToUtf8(const std::u32string& utf32_str);

	std::string Char32ToUtf8(char32_t character);

	size_t GetUtf8StrLength(std::string str);

	int GetGraphemeType(char32_t grapheme);

	bool IsBreakBetween(char32_t l, char32_t r);

	bool IsControl(char32_t ch);

	std::string ToStringP(float val, int precision);


	enum COLOR
	{
		BLACK = 0,
		BLUE = 1,
		GREEN,
		CYAN,
		RED,
		MAGENTA,
		BROWN,
		LIGHTGRAY,
		DARKGRAY,
		LIGHTBLUE,
		LIGHTGREEN,
		LIGHTCYAN,
		LIGHTRED,
		LIGHTMAGENTA,
		YELLOW,
		WHITE
	};

	struct color
	{
	private:
		uint8_t m_foreground :4;
		uint8_t m_background :4;
	public:
		color() : color(tui::COLOR::WHITE, tui::COLOR::BLACK) {}
		color(uint8_t foreground) : color(foreground, tui::COLOR::BLACK) {}
		color(uint8_t foreground, uint8_t background) { setColor(foreground, background); }
		void setColor(uint8_t foreground, uint8_t background)
		{
			m_background = background;
			m_foreground = foreground;
		}

		//swaps foreground color with background color
		void invert()
		{
			uint8_t foreground = m_foreground;
			m_foreground = m_background;
			m_background = foreground;
		}

		int getRGBIColor() { return 16 * m_background + m_foreground; } //return windows console color
		uint8_t getForegroundColor() { return m_foreground; }
		uint8_t getBackgroundColor() { return m_background; }
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
			return c.m_background == m_background && c.m_foreground == m_foreground;
		}
		bool operator!=(color c) { return !operator==(c); }
	};

	struct symbol
	{
	private:
#ifndef TUI_DISABLE_MEMBER_PACKING
#pragma pack(push,1)
#endif
		union grapheme_cluster
		{
			char32_t single_cp;
			char32_t* multiple_cp;
		} m_cluster;

		uint8_t m_multiple_cp_size;

		color m_color;

#ifndef TUI_DISABLE_MEMBER_PACKING
#pragma pack(pop)
#endif

		void setSingleCp(char32_t cp)
		{
			if (isMultipleCP())
			{
				delete[] m_cluster.multiple_cp;
			}

			m_cluster.single_cp = cp;
			m_multiple_cp_size = 0;
		}

		void setMultipleCP(const char32_t* cp, size_t size)
		{
			if (m_multiple_cp_size != size)
			{
				if (isMultipleCP()) { delete[] m_cluster.multiple_cp; }

				m_cluster.multiple_cp = new char32_t[size];
			}
			m_multiple_cp_size = size;

			for (int i = 0; i < m_multiple_cp_size; i++)
			{
				m_cluster.multiple_cp[i] = cp[i];
			}
		}

		bool isMultipleCP() const { return m_multiple_cp_size > 0; }

		
	public:
		symbol() : symbol(U' ', color()) {}
		symbol(const char* Symbol) : symbol(std::string(Symbol), color()) {}
		symbol(const char32_t* Symbol) : symbol(std::u32string(Symbol), color()) {}
		symbol(const char* Symbol, color color) : symbol(std::string(Symbol), color) {}
		symbol(const char32_t* Symbol, color color) : symbol(std::u32string(Symbol), color) {}
		symbol(std::string Symbol) : symbol(Symbol, color()) {}
		symbol(std::u32string Symbol) : symbol(Symbol, color()) {}
		symbol(std::string Symbol, color color) : symbol(Utf8ToUtf32(Symbol), color) {}
		symbol(std::u32string Symbol, color color) : m_multiple_cp_size(0)
		{
			setSymbol(Symbol); 
			setColor(color);
		}
		symbol(char32_t character) : symbol(character, color()) {}
		symbol(char32_t character, color color) : m_multiple_cp_size(0)
		{
			setSingleCp(character);
			setColor(color);
		}

		symbol(const symbol& sym)
		{
			switch (sym.isMultipleCP())
			{
			case false:
				m_cluster.single_cp = sym.m_cluster.single_cp;
				m_multiple_cp_size = 0;
				break;
			case true:
				m_cluster.multiple_cp = new char32_t[sym.m_multiple_cp_size];
				for (int i = 0; i < sym.m_multiple_cp_size; i++)
				{
					m_cluster.multiple_cp[i] = sym.m_cluster.multiple_cp[i];
				}
				m_multiple_cp_size = sym.m_multiple_cp_size;
			}

			m_color = sym.m_color;
		}

		~symbol()
		{
			if (isMultipleCP()) { delete[] m_cluster.multiple_cp; }
		}

		symbol& operator=(const symbol& sym)
		{
			if (this != &sym)
			{
				switch (sym.isMultipleCP())
				{
				case false:
					setSingleCp(sym.m_cluster.single_cp);
					break;
				case true:
					setMultipleCP(sym.m_cluster.multiple_cp, sym.m_multiple_cp_size);
				}

				m_color = sym.m_color;
			}

			return *this;
		}


		void setSymbol(char32_t character) { setSingleCp(character); }

		void setSymbol(std::u32string symbol) 
		{

			if (symbol.size() == 0) { setSingleCp(U' '); }
			else if (symbol.size() == 1) { setSingleCp(symbol[0]); }
			else
			{
				std::u32string temp;
				temp += symbol[0];

				for (int i = 0; i < symbol.size() - 1; i++)
				{
					if (!IsBreakBetween(symbol[i], symbol[i + 1]))
					{
						temp += symbol[i + 1];
					}
					else { break; }
				}

				if (temp.size() > 1)
				{
					setMultipleCP(temp.data(), temp.size());
				}
				else { setSingleCp(temp[0]); }
			}
		}

		void invert() { m_color.invert(); }


		char32_t getFirstChar() const
		{ 
			switch (isMultipleCP())
			{
			case true:
				return m_cluster.multiple_cp[0];
				break;
			case false:
				return m_cluster.single_cp;
			}
		}
		void setColor(color color) { m_color = color; }
		std::u32string getSymbol() const
		{
			std::u32string temp;
			if (!isMultipleCP())
			{
				temp.resize(1);
				temp[0] = m_cluster.single_cp;
			}
			else
			{
				temp.resize(m_multiple_cp_size);
				for (int i = 0; i < m_multiple_cp_size; i++)
				{
					temp[i] = m_cluster.multiple_cp[i];
				}
			}

			return temp;
		}
		color getColor() const { return m_color; }

		bool operator==(const symbol& sym)
		{
			switch (sym.isMultipleCP())
			{
			case false:
				return m_cluster.single_cp == sym.m_cluster.single_cp && m_color == sym.m_color;
			case true:
				if (m_multiple_cp_size != sym.m_multiple_cp_size) { return false; }
				else
				{
					for (int i = 0; i < m_multiple_cp_size; i++)
					{
						if (m_cluster.multiple_cp[i] != sym.m_cluster.multiple_cp[i]) { return false; }
					}
				}
				return m_color == sym.m_color;
			}
		}
		bool operator!=(const symbol& sym) { return !operator==(sym); }
	};


	struct console_string : std::vector<symbol>
	{
	private:
		color m_selected_color;
	public:
		console_string() {}
		console_string(symbol Symbol)
		{
			this->resize(1);
			(*this)[0] = Symbol;
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

			(*(std::vector<symbol>*)this) = temp_vec;
		}

		void operator<< (color color)
		{
			m_selected_color = color;
		}

		void operator<< (const console_string &string)
		{
			for (int i = 0; i < string.size(); i++)
			{
				this->push_back(string[i]);
				(*this)[this->size()-1].setColor(m_selected_color);
			}
		}

		void operator+=(const console_string &str)
		{
			for (int i = 0; i < str.size(); i++)
			{
				this->push_back(str[i]);
			}
		}

		void invert()
		{
			for (int i = 0; i < size(); i++)
			{
				(*this)[i].invert();
			}
		}

		void setColor(color Color)
		{
			for (int i = 0; i < size(); i++)
			{
				(*this)[i].setColor(Color);
			}
		}
	};


	inline std::u32string Utf8ToUtf32(const std::string& utf8_str)
	{
		std::u32string utf32_str;
		utf32_str.reserve(utf8_str.size() / 2);

		char32_t utf32_char = 0;
		int bytec = 0;
		int bitc = 0;

		auto setBc = [&](int bc)
		{
			bytec = bc;
			bitc = (bytec - 1) * 6;
	};

		auto push = [&](int i)
		{
			if (i > 0)
			{
				if (bytec == 0 && !(utf32_char >= 0xD800 && utf32_char <= 0xDFFF) && utf32_char <= 0x10FFFF)
				{
					utf32_str.push_back(utf32_char);
				}
				else { utf32_str.push_back(U'\xFFFD'); }
			}
		};

		for (int i = 0; i < utf8_str.size(); i++, bytec--)
		{
			if ((utf8_str[i] & 0x10000000) == 0b00000000)
			{
				push(i);
				setBc(1);
				utf32_char = utf8_str[i];
			}
			else if ((utf8_str[i] & 0b11000000) == 0b10000000)
			{
				bitc -= 6;
				utf32_char |= (utf8_str[i] & 0b00111111) << bitc;
			}
			else if ((utf8_str[i] & 0b11100000) == 0b11000000)
			{
				push(i);
				setBc(2);
				utf32_char = (utf8_str[i] & 0b000111111) << bitc;
			}
			else if ((utf8_str[i] & 0b11110000) == 0b11100000)
			{
				push(i);
				setBc(3);
				utf32_char = (utf8_str[i] & 0b00001111) << bitc;
			}
			else if ((utf8_str[i] & 0b11111000) == 0b11110000)
			{
				push(i);
				setBc(4);
				utf32_char = (utf8_str[i] & 0b00000111) << bitc;
			}
			else
			{
				push(i);
				setBc(-1);
			}
		}

		if (utf8_str.size() > 0) { push(true); }

		utf32_str.shrink_to_fit();
		return utf32_str;
	}

	inline std::string Utf32ToUtf8(const std::u32string& utf32_str)
	{
		std::string utf8_str;
		utf8_str.reserve(utf32_str.size() * 2);

		char utf8_buf[4] = { 0, 0, 0, 0 };

		auto push = [&](size_t len)
		{
			utf8_str.append((char*)utf8_buf, len);
			((char32_t*)utf8_buf)[0] = 0;
		};

		for (int i = 0; i < utf32_str.size(); i++)
		{
			if (!(utf32_str[i] >= 0xD800 && utf32_str[i] <= 0xDFFF) && utf32_str[i] <= 0x10FFFF)
			{
				if (utf32_str[i] < (1 << 7))
				{
					utf8_buf[0] = utf32_str[i];
					push(1);
				}
				else if (utf32_str[i] < (1 << 11))
				{
					utf8_buf[0] = (utf32_str[i] >> 6) | 0b11000000;
					utf8_buf[1] = (utf32_str[i] & 0b00111111) | 0b10000000;
					push(2);
				}
				else if (utf32_str[i] < (1 << 16))
				{
					utf8_buf[0] = (utf32_str[i] >> 12) | 0b11100000;
					utf8_buf[1] = ((utf32_str[i] >> 6) & 0b00111111) | 0b10000000;
					utf8_buf[2] = (utf32_str[i] & 0b00111111) | 0b10000000;
					push(3);
				}
				else
				{
					utf8_buf[0] = (utf32_str[i] >> 18) | 0b11110000;
					utf8_buf[1] = ((utf32_str[i] >> 12) & 0b00111111) | 0b10000000;
					utf8_buf[2] = ((utf32_str[i] >> 6) & 0b00111111) | 0b10000000;
					utf8_buf[3] = (utf32_str[i] & 0b00111111) | 0b10000000;
					push(4);
				}
			}
			else
			{
				utf8_str.append("\xEF\xBF\xBD");
			}

		}

		utf8_str.shrink_to_fit();
		return utf8_str;
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


	namespace GRAPHEME_TYPE
	{
		enum GRAPHEME_TYPE
		{
			OTHER,
			CONTROL,
			LF,
			CR,
			EXTEND,
			PREPEND,
			SPACINGMARK,
			REGIONAL_INDICATOR,
			L,
			V,
			T,
			LV,
			LVT,
			ZWJ
		};
	}

	inline int GetGraphemeType(char32_t grapheme)
	{
#ifndef TUI_DISABLE_MEMBER_PACKING
#pragma pack(push,1)
#endif
		struct grapheme_range_info { unsigned int r_s, r_e; uint8_t r_t; };

#ifndef TUI_DISABLE_MEMBER_PACKING
#pragma pack(pop)
#endif

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

	inline bool IsControl(char32_t ch)
	{
		int g_type = GetGraphemeType(ch);

		return g_type == GRAPHEME_TYPE::CONTROL
			|| g_type == GRAPHEME_TYPE::CR
			|| g_type == GRAPHEME_TYPE::LF;
	}

	std::string ToStringP(float val, int precision)
	{
		std::stringstream ss_val;
		if (precision >= 0)
		{
			ss_val << std::fixed << std::setprecision(precision);
		}
		ss_val << val;
		std::string s_val = ss_val.str();

		if (precision > 0)
		{
			for (int i = s_val.size() - 1; i > 0; i--)
			{
				if (s_val[i] == '0') { s_val.pop_back(); }
				else if (s_val[i] == '.')
				{
					s_val.pop_back();
					break;
				}
				else { break; }
			}
		}

		return s_val;
	}
}
