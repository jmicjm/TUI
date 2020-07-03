#pragma once

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
		vec2i() : vec2i(0,0) {}
		vec2i(int X, int Y) : x(X), y(Y) {}

		bool operator==(vec2i r)
		{
			return (x == r.x && y == r.y);
		}
		bool operator!=(vec2i r)
		{
			return (x != r.x || y != r.y);
		}

		vec2i& operator+=(vec2i r)
		{
			x += r.x;
			y += r.y;
			return *this;
		}
		vec2i& operator-=(vec2i r)
		{
			x -= r.x;
			y -= r.y;
			return *this;
		}

		vec2i operator+(vec2i r)
		{
			return { x + r.x, y + r.y };
		}
		vec2i operator-(vec2i r)
		{
			return { x - r.x, y - r.y };
		}
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
	public:
		time_frame(std::chrono::milliseconds frame_time = std::chrono::milliseconds(100))
		{
			setFrameTime(frame_time);
			start();
		}

		void restart()
		{
			stop();
			start();
		}

		void setFrameTime(std::chrono::milliseconds frame_time)
		{
			m_frame_time = frame_time;
			restart();
		}

		bool isEnd(bool restart_clock = false)
		{
			bool is_end = std::chrono::steady_clock::now() - m_frame_start_point >= m_frame_time;

			if (is_end && restart_clock) { restart(); }

			return is_end;
		}

		void sleepUntilEnd()
		{
			std::this_thread::sleep_until(m_frame_start_point + m_frame_time);
			restart();
		}
	};

}