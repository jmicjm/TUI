#pragma once

#include "tui_config.h"

#include "tui_enums.h"

#include <string>
#include <vector>
#include <array>
#include <chrono>
#include <thread>

#include <locale>
#include <codecvt>


namespace tui 
{
	struct vec2i
	{
		int x, y;
		vec2i() {}
		vec2i(int X, int Y)
		{
			x = X;
			y = Y;
		}

		bool operator==(vec2i r)
		{
			if (x == r.x && y == r.y)
			{ return true; }
			else { return false; }
		}
		bool operator!=(vec2i r)
		{
			if (x != r.x || y != r.y)
			{ return true; }
			else { return false; }
		}
	};

	struct surface_size
	{
	private:
		vec2i m_fixed_size;
		vec2i m_percentage_size;

	public:
		surface_size() : surface_size({ 0,0 }, { 0,0 }) {}
		surface_size(vec2i size) : surface_size(size, { 0,0 }) {}
		surface_size(vec2i fixed, vec2i percentage)
		{
			setSize(fixed, percentage);
		}

		void setSize(vec2i fixed, vec2i percentage)
		{
			m_fixed_size = fixed;
			m_percentage_size = percentage;
		}
		vec2i getFixedSize() { return m_fixed_size; }
		vec2i getPercentagesize() { return m_percentage_size; }

	};

	struct position
	{
		private:
			vec2i m_offset;
			vec2i m_percentage_offset;
			vec2i m_relative;
		public:
			position() : position(vec2i(0, 0), vec2i(0,0), vec2i(POSITION::HORIZONTAL::LEFT, POSITION::VERTICAL::TOP)) {}
			position(vec2i offset): position(offset, vec2i(0,0), vec2i(POSITION::HORIZONTAL::LEFT, POSITION::VERTICAL::TOP)) {}
			position(vec2i offset, vec2i percentage_offset) : position(offset, percentage_offset, vec2i(POSITION::HORIZONTAL::LEFT, POSITION::VERTICAL::TOP)) {}
			position(vec2i offset, vec2i percentage_offset, vec2i relative_point)
			{
				setOffset(offset);
				setRelativePoint(relative_point);
				setPercentageOffset(percentage_offset);
			}
			void setOffset(vec2i offset) { m_offset = offset; }
			void setPercentageOffset(vec2i percentage_offset) { m_percentage_offset = percentage_offset; }
			void setRelativePoint(vec2i relative) { m_relative = relative; }

			vec2i getOffset() { return m_offset; }
			vec2i getPercentageOffset() { return m_percentage_offset; }
			vec2i getRelativePoint() { return m_relative; }
	};


	struct time_frame
	{
	private:
		std::chrono::milliseconds m_frame_time;
		std::chrono::steady_clock::time_point m_frame_start_point;
		std::chrono::steady_clock::time_point m_frame_end_point;

		void start()
		{
			m_frame_start_point = std::chrono::steady_clock::now();
		}
		void stop()
		{
			m_frame_end_point = std::chrono::steady_clock::now();
		}
		void restart()
		{
			stop();
			start();
		}

	public:
		time_frame(std::chrono::milliseconds frame_time)
		{
			setFrameTime(frame_time);
			start();
		}

		void setFrameTime(std::chrono::milliseconds frame_time)
		{
			m_frame_time = frame_time;
		}

		bool isEnd(bool restart_clock)
		{
			if (std::chrono::steady_clock::now() - m_frame_start_point >= m_frame_time)
			{
				if (restart_clock == true)
				{
					restart();
				}
				return true; 
			}
			else { return false; }
		}

		void sleepUntilEnd()
		{
			std::this_thread::sleep_until(m_frame_start_point + m_frame_time);
			restart();
		}

	};

}