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
		std::function<bool()> block_function;

		navigation_group_entry(active_element* element, std::function<bool()> block_function = nullptr) : element(element), block_function(block_function) {}
	};

	struct navigation_group : active_element, private std::vector<navigation_group_entry>
	{
	private:
		size_t m_selected = 0;

		void disable(int except = -1)
		{
			for (int i = 0; i < size(); i++)
			{
				if ((*this)[i].element != nullptr)
				{
					if ((*this)[i].element->isActive() && i != except)
					{
						(*this)[i].element->disactivate();
					}
				}
			}
		}
		void enable()
		{
			m_selected = size() > 0 ? std::min(m_selected, size()-1) : 0;
			disable(m_selected);

			if ((*this)[m_selected].element != nullptr)
			{
				if (size() > 0 && !(*this)[m_selected].element->isActive())
				{
					(*this)[m_selected].element->activate();
				}
			}
		}

		bool isBlocked()
		{
			if ((*this)[m_selected].block_function)
			{
				return (*this)[m_selected].block_function();
			}
			return false;
		}

		void disactivationAction() override { disable(); }
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

		void update()
		{
			if (isActive())
			{		
				if (!isBlocked())
				{
					if (input::isKeyPressed(key_next))
					{
						m_selected < size() - 1 ? m_selected++ : m_selected = 0;
					}
					if (input::isKeyPressed(key_prev))
					{
						m_selected > 0 ? m_selected-- : m_selected = size() - 1;
					}

					enable();
				}
			}
		}
	};
}