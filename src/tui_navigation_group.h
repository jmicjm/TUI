#pragma once
#include "tui_active_element.h"
#include "tui_input.h"

#include <vector>
#include <algorithm>

namespace tui
{
	struct navigation_group : active_element, private std::vector<active_element*>
	{
	private:
		size_t m_selected = 0;
		bool m_blocked = false;

		void disable(int except = -1)
		{
			for (int i = 0; i < size(); i++)
			{
				if ((*this)[i]->isActive() && i != except)
				{
					(*this)[i]->disactivate();
				}
			}
		}
		void enable()
		{
			m_selected = size() > 0 ? std::min(m_selected, size()-1) : 0;
			disable(m_selected);

			if (size() > 0 && !(*this)[m_selected]->isActive())
			{
				(*this)[m_selected]->activate();
			}
		}

		void disactivationAction() override { disable(); }
		void activationAction() override { enable(); }
	public:
		short key_next = input::KEY::CTRL_RIGHT;
		short key_prev = input::KEY::CTRL_LEFT;
		short key_block = -1;

		navigation_group() {}
		navigation_group(active_element* element) : std::vector<active_element*>({ element }) {}
		navigation_group(std::vector<active_element*> elements) : std::vector<active_element*>(elements) {}

		using std::vector<active_element*>::size;
		using std::vector<active_element*>::operator[];
		using std::vector<active_element*>::push_back;
		using std::vector<active_element*>::pop_back;
		using std::vector<active_element*>::insert;
		using std::vector<active_element*>::begin;
		using std::vector<active_element*>::end;
		using std::vector<active_element*>::clear;
		using std::vector<active_element*>::erase;

		void update()
		{
			if (isActive())
			{
				if (input::isKeyPressed(key_block))
				{
					m_blocked = !m_blocked;
				}

				if (!m_blocked)
				{
					if (input::isKeyPressed(key_next))
					{
						m_selected < size() - 1 ? m_selected++ : m_selected = 0;
					}
					if (input::isKeyPressed(key_prev))
					{
						m_selected > 0 ? m_selected-- : m_selected = size()-1;					
					}			
				}
				enable();
			}
		}
	};
}