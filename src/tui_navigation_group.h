#pragma once
#include "tui_utils.h"
#include "tui_active_element.h"
#include "tui_input.h"

#include <vector>


namespace tui
{
	struct navigation_group : active_element
	{
	private:
		std::vector<active_element*> m_elements;
		int m_selected = -1;
		bool m_blocked = false;

		void disactivateAll()
		{
			for (int i = 0; i < m_elements.size(); i++)
			{
				m_elements[i]->disactivate();
			}
		}
		void disactivationAction() { disactivateAll(); }
		void activationAction()
		{
			disactivateAll();
			m_blocked = false;

			if (m_selected > m_elements.size() - 1)
			{
				m_selected = m_elements.size() - 1;
			}

			if (m_elements.size() > 0)
			{
				m_elements[m_selected]->activate();
			}
		}
	public:
		int key_next = -1;
		int key_prev = -1;
		int key_block = -1;

		void addElement(active_element& element)
		{
			m_elements.push_back(&element);
		}

		void removeElement(active_element& element)
		{
			for (int i = 0; i < m_elements.size(); i++)
			{
				if (m_elements[i] == &element)
				{
					m_elements.erase(m_elements.begin() + i);

					if (m_selected == i)
					{
						m_selected = 0;
					}
				}
			}
		}

		void update()
		{
			if (isActive())
			{
				if (tui::input::isKeyPressed(key_block))
				{
					m_blocked = !m_blocked;
				}

				if (tui::input::isKeyPressed(key_next) && !m_blocked)
				{
					disactivateAll();

					if (m_selected < m_elements.size() - 1) { m_selected++; }
					else { m_selected = 0; }

					m_elements[m_selected]->activate();
				}

				if (tui::input::isKeyPressed(key_prev) && !m_blocked)
				{
					disactivateAll();

					if (m_selected > 0) { m_selected--; }
					else { m_selected = m_elements.size() - 1; }

					m_elements[m_selected]->activate();
				}
			}
		}
	};
}