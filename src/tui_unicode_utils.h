#pragma once
#include <string>
#include <vector>

namespace tui
{
	std::u32string Utf8ToUtf32(const std::string& utf8_str);

	std::string Utf32ToUtf8(const std::u32string& utf32_str);

	std::string Char32ToUtf8(char32_t character);

	size_t GetUtf8StrLength(std::string str);

	int GetGraphemeType(char32_t grapheme);

	bool IsBreakBetween(char32_t l, char32_t r);

	bool IsControl(char32_t ch);


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
		if (l_t == GRAPHEME_TYPE::CONTROL
			|| l_t == GRAPHEME_TYPE::CR
			|| l_t == GRAPHEME_TYPE::LF)
		{
			return true;
		}
		//GB5
		if (r_t == GRAPHEME_TYPE::CONTROL
			|| r_t == GRAPHEME_TYPE::CR
			|| r_t == GRAPHEME_TYPE::LF)
		{
			return true;
		}
		//GB6
		if (l_t == GRAPHEME_TYPE::L
			&&
			(r_t == GRAPHEME_TYPE::L
				|| r_t == GRAPHEME_TYPE::V
				|| r_t == GRAPHEME_TYPE::LV
				|| r_t == GRAPHEME_TYPE::LVT)
			)
		{
			return false;
		}
		//GB7
		if ((l_t == GRAPHEME_TYPE::LV
			|| l_t == GRAPHEME_TYPE::V)
			&&
			(r_t == GRAPHEME_TYPE::V
				|| r_t == GRAPHEME_TYPE::T)
			)
		{
			return false;
		}
		//GB8
		if ((l_t == GRAPHEME_TYPE::LVT
			|| l_t == GRAPHEME_TYPE::T)
			&&
			r_t == GRAPHEME_TYPE::T
			)
		{
			return false;
		}
		//GB9
		if (r_t == GRAPHEME_TYPE::EXTEND
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

	inline bool IsControl(char32_t ch)
	{
		int g_type = GetGraphemeType(ch);

		return g_type == GRAPHEME_TYPE::CONTROL
			|| g_type == GRAPHEME_TYPE::CR
			|| g_type == GRAPHEME_TYPE::LF;
	}

}
