#pragma once
#include "tui_unicode_utils.h"
#include "tui_color.h"

#include <locale>
#include <codecvt>
#include <string>
#include <vector>
#include <array>
#include <sstream>
#include <iomanip>

namespace tui
{
	std::string toStringP(float val, int precision);

	enum class ATTRIBUTE
	{
		UNDERSCORE,
		NO_UNDERSCORE
	};

	struct symbol
	{
	private:

		struct hybrid_container
		{
		private:
			constexpr static unsigned int stack_size = sizeof(char*);

#ifndef TUI_DISABLE_MEMBER_PACKING
#pragma pack(push,1)
#endif
			union
			{
				char m_stack[stack_size];
				char* m_heap;
			};
			uint8_t m_size;
#ifndef TUI_DISABLE_MEMBER_PACKING
#pragma pack(pop)
#endif

			bool isStack() const  { return m_size <= stack_size; }
			bool isHeap() const { return !isStack(); }

			void clearStack()
			{
				m_heap = 0;
			}
			
			void setStackData(const char* data, uint8_t size)
			{
				if(isHeap()) { delete[] m_heap; }

				clearStack();

				for (int i = 0; i < size; i++)
				{
					m_stack[i] = data[i];
				}
				m_size = size;
			}
			void setHeapData(const char* data, uint8_t size)
			{
				if (isStack()) { m_heap = new char[size]; }

				if (isHeap() && m_size != size)
				{
					delete[] m_heap;
					m_heap = new char[size];
				}

				for (int i = 0; i < size; i++)
				{
					m_heap[i] = data[i];
				}
				m_size = size;
			}
		public:
			hybrid_container()
			{
				clearStack();
				m_size = 0;
			}
			~hybrid_container()
			{
				if (isHeap()) { delete[] m_heap; }
			}

			hybrid_container& operator=(const hybrid_container& other)
			{
				if (this != &other)
				{
					setData(other.getData(), other.size());
				}
				return *this;
			}

			uint8_t size() const { return m_size; }

			const char* getData() const 
			{
				switch (isStack())
				{
				case true:
					return m_stack;
				case false:
					return m_heap;
				}
			}

			void setData(const char* data, uint8_t size)
			{
				switch (size <= stack_size)
				{
				case true:
					setStackData(data, size);
					break;
				case false:
					setHeapData(data, size);
				}
			}

			bool operator==(const hybrid_container& other) const
			{
				if (size() != other.size()) { return false; }
				
				for (int i = 0; i < size(); i++)
				{
					if(getData()[i] != other.getData()[i])
					{
						return false;
					}
				}
				return true;
			}
			bool operator!=(const hybrid_container& other) const { return !operator==(other); }
		} m_cluster;

		bool m_underscore = false;
		color m_color;

	public:
		symbol() : symbol("", color()) {}
		symbol(char32_t c, color Color = color()) : symbol(utf32ToUtf8(std::u32string(&c,1)), Color) {}
		symbol(const char* cluster, color color = color()) : symbol(std::string(cluster), color) {}
		symbol(const char32_t* cluster, color color = color()) : symbol(utf32ToUtf8(std::u32string(cluster)), color) {}

		symbol(const std::string& cluster, color Color = color())
		{
			setCluster(cluster);
			setColor(Color);
		}

		symbol(const symbol& other)
		{
			m_cluster = other.m_cluster;
			m_color = other.m_color;
			m_underscore = other.m_underscore;
		}

		symbol& operator=(const symbol& other)
		{
			m_cluster = other.m_cluster;
			m_color = other.m_color;
			m_underscore = other.m_underscore;

			return *this;
		}

		uint8_t size() const { return m_cluster.size(); }

		const char& operator[](int i) const
		{
			return m_cluster.getData()[i];
		}

		void setCluster(const std::string& cluster)
		{
			if (cluster.size() == 0)
			{
				m_cluster.setData(nullptr, 0);
				return;
			}

			std::u32string utf32_cluster = utf8ToUtf32(cluster);
			unsigned int size = 1;
			
			for (; size < utf32_cluster.size(); size++)
			{
				if (isBreakBetween(utf32_cluster[size - 1], utf32_cluster[size]))
				{
					break;
				}
			}

			utf32_cluster.resize(size);

			std::string new_cluster = utf32ToUtf8(utf32_cluster);

			m_cluster.setData(new_cluster.data(), new_cluster.size());
		}
		std::string getCluster() const
		{
			return std::string(m_cluster.getData(), m_cluster.size());
		}

		void invert() { m_color.invert(); }

		void setColor(color Color) { m_color = Color; }
		color getColor() { return m_color; }
		void setUnderscore(bool set) { m_underscore = set; }
		bool isUnderscore() { return m_underscore; }

		bool operator==(const symbol& other)
		{
			return m_cluster == other.m_cluster && m_color == other.m_color && m_underscore == other.m_underscore;
		}
		bool operator!=(const symbol& other) { return !operator==(other); }
	};

	struct console_string : std::vector<symbol>
	{
	private:
		color m_selected_color;
		bool m_underscore = false;
	public:
		console_string() {}
		console_string(symbol Symbol)
		{
			this->resize(1);
			(*this)[0] = Symbol;
		}
		console_string(const char* str) : console_string(utf8ToUtf32(str)) {}
		console_string(const char32_t* str) : console_string(std::u32string(str)) {}
		console_string(const std::string& str) : console_string(utf8ToUtf32(str), color()) {}
		console_string(const std::u32string& str) : console_string(str, color()) {}
		console_string(const std::string& str, color color) : console_string(utf8ToUtf32(str), color) {}	
		console_string(const std::u32string& str, color color)
		{
			std::vector<symbol> temp_vec;

			if (str.size() > 0)
			{
				int i = 0;
				while (i < str.size())
				{
					std::u32string temp;
					temp += str[i];

					for (; (i<str.size()-1 && !isBreakBetween(str[i], str[i + 1])); i++)
					{
						temp += str[i + 1];
					}			
					temp_vec.push_back(symbol(utf32ToUtf8(temp), color));
					i++;
				}
			}

			(*(std::vector<symbol>*)this) = temp_vec;
		}

		console_string& operator<< (color color)
		{
			m_selected_color = color;
			return *this;
		}

		console_string& operator<<(ATTRIBUTE attr)
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

		console_string& operator<< (const console_string& string)
		{
			for (int i = 0; i < string.size(); i++)
			{
				this->push_back(string[i]);
				(*this)[this->size()-1].setColor(m_selected_color);
				(*this)[this->size()-1].setUnderscore(m_underscore);
			}
			return *this;
		}

		console_string& operator+=(const console_string& str)
		{
			for (int i = 0; i < str.size(); i++)
			{
				this->push_back(str[i]);
			}
			return *this;
		}

		console_string operator+(const console_string& str)
		{
			console_string tmp = *this;
			return tmp += str;
		}

		void setSelectedColor(color Color) { m_selected_color = Color; }
		void setSelectedUnderscore(bool underscore) { m_underscore = underscore; }

		color getSelectedColor() { return m_selected_color; }
		bool isSelectedUnderscore() { return m_underscore; }

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

	inline bool isPunctuation(symbol Symbol)
	{
		return   Symbol == U"."
			|| Symbol == U","
			|| Symbol == U":"
			|| Symbol == U";"
			|| Symbol == U"!"
			|| Symbol == U"?";
	}

	inline std::string toStringP(float val, int precision)
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
