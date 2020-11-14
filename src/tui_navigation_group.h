//this file contains navigation_group that could be used to simplyfy content navigation
#pragma once
#include "tui_active_element.h"
#include "tui_input.h"

#include <vector>
#include <algorithm>
#include <functional>

namespace tui
{
	struct navigation_group_entry
	{
		active_element* element;
		std::function<bool()> unblock_check;//should return true to unblock, if not present blocking wont be used
		std::function<bool()> block_check;//should return true to block

		navigation_group_entry(
			active_element* element,
			std::function<bool()> unblock_check = nullptr,
			std::function<bool()> block_check = nullptr
		)
			: element(element),
			unblock_check(unblock_check),
			block_check(block_check) {}
	};

	struct navigation_group : active_element, private std::vector<navigation_group_entry>
	{
	private:
		size_t m_selected = 0;
		bool m_blocked = false;
		bool m_wrap_around = true;

		void disable(int except = -1)
		{
			for (int i = 0; i < size(); i++)
			{
				if ((*this)[i].element != nullptr)
				{
					if ((*this)[i].element->isActive() && i != except)
					{
						(*this)[i].element->deactivate();
					}
				}
			}
		}
		void enable()
		{
			m_selected = size() > 0 ? std::min(m_selected, size()-1) : 0;
			disable(m_selected);

			if (size() > 0)
			{
				if ((*this)[m_selected].element != nullptr)
				{
					if (!(*this)[m_selected].element->isActive())
					{
						(*this)[m_selected].element->activate();
					}
					
					m_blocked = (bool)((*this)[m_selected].unblock_check);//block only if there is way to unblock
				}
			}
		}

		void updateBlocked()
		{
			m_selected = size() > 0 ? std::min(m_selected, size() - 1) : 0;
			if (size() > 0)
			{
				if ((*this)[m_selected].unblock_check)
				{
					if ((*this)[m_selected].unblock_check())
					{
						m_blocked = false;
					}
					if ((*this)[m_selected].block_check && (*this)[m_selected].block_check())
					{
						m_blocked = true;
					}
				}
				else
				{
					m_blocked = false;
				}
			}
		}

		void deactivationAction() override { disable(); }
		void activationAction() override { enable(); }
	public:
		short key_next = input::KEY::RIGHT;
		short key_prev = input::KEY::LEFT;

		navigation_group() {}
		navigation_group(navigation_group_entry element) : std::vector<navigation_group_entry>({ element }) {}
		navigation_group(std::vector<navigation_group_entry> elements) : std::vector<navigation_group_entry>(elements) {}

		using std::vector<navigation_group_entry>::size;
		using std::vector<navigation_group_entry>::operator[];
		using std::vector<navigation_group_entry>::push_back;
		using std::vector<navigation_group_entry>::pop_back;
		using std::vector<navigation_group_entry>::insert;
		using std::vector<navigation_group_entry>::begin;
		using std::vector<navigation_group_entry>::end;
		using std::vector<navigation_group_entry>::clear;
		using std::vector<navigation_group_entry>::erase;

		void useWrappingAround(bool use) { m_wrap_around = use; }
		bool isUsingWrappingAround() { return m_wrap_around; }

		void setSelected(unsigned int selected)
		{
			m_selected = selected;
			if (isActive()) { enable(); }
		}
		unsigned int getSelected() { return m_selected; }

		void next()
		{
			m_selected = m_selected < size() - 1 ? m_selected + 1 : (m_wrap_around ? 0 : m_selected);
			enable();
		}
		void prev()
		{
			m_selected = m_selected > 0 ? m_selected - 1 : (m_wrap_around ? size() - 1 : m_selected);
			enable();
		}

		void update()
		{
			if (isActive())
			{		
				updateBlocked();
				if (!m_blocked)
				{
					if (input::isKeyPressed(key_next))
					{
						next();
					}
					if (input::isKeyPressed(key_prev))
					{
						prev();
					}
				}
			}
		}
	};
}