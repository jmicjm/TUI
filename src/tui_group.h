#pragma once
#include "tui_surface.h"

#include <vector>


namespace tui
{
	struct group : surface
	{
		std::vector<surface*> m_surfaces;


		group(surface_size size)
		{
			setSizeInfo(size);
		}


		void addSurface(surface& surf)
		{
			m_surfaces.push_back(&surf);
		}

		void removeSurface(surface& surf)
		{
			for (int i = 0; i < m_surfaces.size(); i++)
			{
				if (m_surfaces[i] == &surf)
				{
					m_surfaces.erase(m_surfaces.begin() + i);
				}
			}
		}

		void draw_action()
		{
			makeTransparent();

			for (int i = 0; i < m_surfaces.size(); i++)
			{
				insertSurface(*m_surfaces[i]);
			}
		}

		void resize_action()
		{

		}
	};
}