#pragma once
#include "tui_color.h"
#include "tui_symbol.h"

#include <string>
#include <vector>

namespace tui
{
	enum class ATTRIBUTE
	{
		UNDERSCORE,
		NO_UNDERSCORE
	};

	struct symbol_string : private std::vector<symbol>
	{
	private:
		color m_selected_color;
		bool m_underscore = false;
	public:
		symbol_string() {}
		symbol_string(symbol Symbol)
		{
			resize(1);
			(*this)[0] = Symbol;
		}
		symbol_string(const char* str) : symbol_string(utf8ToUtf32(str)) {}
		symbol_string(const char32_t* str) : symbol_string(std::u32string(str)) {}
		symbol_string(const std::string& str) : symbol_string(utf8ToUtf32(str), color()) {}
		symbol_string(const std::u32string& str) : symbol_string(str, color()) {}
		symbol_string(const std::string& str, color color) : symbol_string(utf8ToUtf32(str), color) {}
		symbol_string(const std::u32string& str, color color)
		{
			std::vector<symbol> temp_vec;

			if (str.size() > 0)
			{
				int i = 0;
				while (i < str.size())
				{
					std::u32string temp;
					temp += str[i];

					for (; (i < str.size() - 1 && !isBreakBetween(str[i], str[i + 1])); i++)
					{
						temp += str[i + 1];
					}
					temp_vec.push_back(symbol(utf32ToUtf8(temp), color));
					i++;
				}
			}

			(*(std::vector<symbol>*)this) = temp_vec;
		}

		using std::vector<symbol>::operator[];
		using std::vector<symbol>::front;
		using std::vector<symbol>::back;
		using std::vector<symbol>::data;
		using std::vector<symbol>::begin;
		using std::vector<symbol>::end;
		using std::vector<symbol>::empty;
		using std::vector<symbol>::size;
		using std::vector<symbol>::clear;
		using std::vector<symbol>::insert;
		using std::vector<symbol>::erase;
		using std::vector<symbol>::push_back;
		using std::vector<symbol>::pop_back;
		using std::vector<symbol>::resize;


		symbol_string& operator<<(color color)
		{
			m_selected_color = color;
			return *this;
		}

		symbol_string& operator<<(ATTRIBUTE attr)
		{
			switch (attr)
			{
			case ATTRIBUTE::UNDERSCORE:
				m_underscore = true;
				break;
			case ATTRIBUTE::NO_UNDERSCORE:
				m_underscore = false;
			}

			return *this;
		};

		symbol_string& operator<<(const symbol_string& other)
		{
			for (int i = 0; i < other.size(); i++)
			{
				push_back(other[i]);
				back().setColor(m_selected_color);
				back().setUnderscore(m_underscore);
			}
			return *this;
		}

		symbol_string& operator+=(const symbol_string& other)
		{
			for (int i = 0; i < other.size(); i++)
			{
				push_back(other[i]);
			}
			return *this;
		}

		bool operator==(const symbol_string& other) const
		{
			return *(std::vector<symbol>*)this == (std::vector<symbol>)other;
		}
		bool operator!=(const symbol_string& other) const { return !operator==(other); }

		void setSelectedColor(color Color) { m_selected_color = Color; }
		void setSelectedUnderscore(bool underscore) { m_underscore = underscore; }

		color getSelectedColor() const { return m_selected_color; }
		bool isSelectedUnderscore() const { return m_underscore; }

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

		void setUnderscore(bool set)
		{
			for (int i = 0; i < size(); i++)
			{
				(*this)[i].setUnderscore(set);
			}
		}
	};

	inline symbol_string operator+(const symbol_string& l, const symbol_string& r)
	{
		symbol_string tmp = l;
		return tmp += r;
	}
}
