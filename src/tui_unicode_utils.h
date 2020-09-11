#pragma once
#include <string>

namespace tui
{
	enum class GRAPHEME_TYPE : uint8_t
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

	GRAPHEME_TYPE getGraphemeType(char32_t grapheme);

	std::u32string utf8ToUtf32(const std::string& utf8_str, bool shrink_after = true, float reserve_ratio = 0.5);
	std::u32string utf8ToUtf32(const char* utf8_str, size_t len, bool shrink_after = true, float reserve_ratio = 0.5);

	std::string utf32ToUtf8(const std::u32string& utf32_str, bool shrink_after = true, float reserve_ratio = 2);
	std::string utf32ToUtf8(const char32_t* utf32_str, size_t len, bool shrink_after = true, float reserve_ratio = 2);

	bool isBreakBetween(char32_t l, char32_t r);

	bool isControl(char32_t cp);

	bool isWide(char32_t cp);
}