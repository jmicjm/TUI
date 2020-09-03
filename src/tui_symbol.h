#pragma once
#include "tui_unicode_utils.h"
#include "tui_color.h"

#include <algorithm>
#include <string>

namespace tui
{
	struct symbol
	{
	private:

		//stores small amount of data in place of pointer
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
			uint8_t m_size = 0;
#ifndef TUI_DISABLE_MEMBER_PACKING
#pragma pack(pop)
#endif

			bool isStack() const { return m_size <= stack_size; }
			bool isHeap() const { return !isStack(); }

			void clearStack()
			{
				m_heap = 0;
			}

			void setStackData(const char* data, uint8_t size)
			{
				if (isHeap()) { delete[] m_heap; }

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
			}
			hybrid_container(const hybrid_container& other)
			{
				setData(other.getData(), other.size());
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
					if (getData()[i] != other.getData()[i])
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
		symbol(char32_t c, color Color = color()) : symbol(utf32ToUtf8(std::u32string(&c, 1)), Color) {}
		symbol(const char* cluster, color color = color()) : symbol(std::string(cluster), color) {}
		symbol(const char32_t* cluster, color color = color()) : symbol(utf32ToUtf8(std::u32string(cluster)), color) {}

		symbol(const std::string& cluster, color Color = color())
		{
			setCluster(cluster);
			setColor(Color);
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
		color getColor() const { return m_color; }
		void setUnderscore(bool set) { m_underscore = set; }
		bool isUnderscore() const { return m_underscore; }

		bool operator==(const symbol& other) const
		{
			return m_cluster == other.m_cluster && m_color == other.m_color && m_underscore == other.m_underscore;
		}
		bool operator!=(const symbol& other) const { return !operator==(other); }
	};

	//returns number of columns occupied by symbol
	inline int symbolWidth(const symbol& sym)
	{
		//probably broken here and there

		std::u32string u32_s = utf8ToUtf32(&sym[0], sym.size(), false);

		if (u32_s.size() == 0 || isControl(u32_s[0]))
		{
			return 0;
		}

		int w = 0;

		for (int i = 0; i < u32_s.size(); i++)
		{
			int c_w = 0;

			if (isWide(u32_s[i]))
			{
				c_w = 2;
			}
			else if (getGraphemeType(u32_s[i]) != GRAPHEME_TYPE::EXTEND)
			{
				c_w = 1;
			}

			w = std::max(w, c_w);
		}

		return w;
	}
}