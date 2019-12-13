#pragma once
#include "tui_utils.h"
#include "tui_surface.h"
#include <vector>
#include <chrono>


namespace tui
{
	struct animation : surface
	{
	private:
		time_frame m_fps_control;
		std::vector<surface> m_frames;
		int m_frame = 0;

		void fill()
		{
			if (m_frames.size() <= 0) { return; }

			if (m_fps_control.isEnd(true)) 
			{
				makeTransparent(); 

				insertSurface(m_frames[m_frame]);

				if (m_frame < m_frames.size() - 1)
				{
					m_frame++;
				}
				else
				{
					m_frame = 0;
				}
			}
		}
		void drawAction() override { fill(); }
	public:
		void addFrame(surface surf)
		{
			m_frames.push_back(surf);
		}
		void setFrames(std::vector<surface> frames)
		{
			m_frames = frames;
			m_frame = 0;
		}
		void clear() 
		{
			m_frames.clear();
			m_frame = 0;
		}
		void setFPS(unsigned int fps) { m_fps_control.setFrameTime(std::chrono::milliseconds(1000)/ fps); }
	};
}