#pragma once
#include "tui_surface.h"
#include "tui_time_frame.h"

#include <vector>


namespace tui
{
	struct animation : surface
	{
	private:
		time_frame m_fps_control;
		std::vector<surface> m_frames;
		int m_frame = -1;

		void fill()
		{
			if (m_frames.size() == 0) { return; }

			if (m_fps_control.isEnd(true)) 
			{
				m_frame < m_frames.size() - 1 ? m_frame++ : m_frame = 0;

				clear(); 
				insertSurface(m_frames[m_frame]);
			}
		}
		void drawAction() override { fill(); }
	public:
		animation() {}
		animation(surface frame) : m_frames({frame}) {}
		animation(std::vector<surface> frames) : m_frames(frames) {}

		void setFrames(std::vector<surface> frames)
		{
			m_frames = frames;
			m_frame = -1;
		}
		std::vector<surface> getFrames() { return m_frames; }

		//returns index of current frame
		int getFrame() { return m_frame; }

		void addFrame(surface frame)
		{
			m_frames.push_back(frame);
		}
		void setFPS(unsigned int fps) { m_fps_control.setFrameTime(std::chrono::milliseconds(1000)/ fps); }
	};
}