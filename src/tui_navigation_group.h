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
		time_frame m_time_limit;


		void disactivateAll()
		{
			for (int i = 0; i < m_elements.size(); i++)
			{
				m_elements[i]->disactivate();
			}
		}

	public:
		int key_next = -1;

		navigation_group() : m_time_limit(std::chrono::milliseconds(300)) {}

		void addElement(active_element& element)
		{
			m_elements.push_back(&element);
		}

		void update()
		{
			if (isActive())
			{
				if (tui::KEYBOARD::isKeyPressed(key_next))
				{
					disactivateAll();

					if (m_selected < (int)m_elements.size() - 1) { m_selected++; }
					else { m_selected = 0; }
					m_elements[m_selected]->activate();
				}
			}
		}

		void disactivationAction() { disactivateAll(); }
		void activationAction()
		{
			disactivateAll();
			m_selected = 0;
			if (m_elements.size() > 0)
			{
				m_elements[m_selected]->activate();
			}
		}

	};
}