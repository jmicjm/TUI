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

	struct surface_size
	{
		vec2i fixed;
		vec2i percentage;

		surface_size() : surface_size({ 0,0 }, { 0,0 }) {}
		surface_size(vec2i Fixed) : surface_size(Fixed, { 0,0 }) {}
		surface_size(vec2i Fixed, vec2i Percentage) : fixed(Fixed), percentage(Percentage) {}
	};

	struct surface1D_size
	{
		int fixed;
		int percentage;

		surface1D_size() : surface1D_size(0,0) {}
		surface1D_size(int Fixed) : surface1D_size(Fixed, 0) {}
		surface1D_size(int Fixed, int Percentage) : fixed(Fixed), percentage(Percentage) {}

	};

	namespace POSITION
	{
		enum POSITION
		{
			BEGIN = 0, CENTER = 50, END = 100
		};
	}

	struct position
	{
		vec2i offset;
		vec2i percentage_offset;
		vec2i relative;

		position() : position(vec2i(0, 0), vec2i(0,0), vec2i(POSITION::BEGIN, POSITION::BEGIN)) {}
		position(vec2i offset): position(offset, vec2i(0,0), vec2i(POSITION::BEGIN, POSITION::BEGIN)) {}
		position(vec2i offset, vec2i percentage_offset) : position(offset, percentage_offset, vec2i(POSITION::BEGIN, POSITION::BEGIN)) {}
		position(vec2i Offset, vec2i Percentage_offset, vec2i Relative) : offset(Offset), percentage_offset(Percentage_offset), relative(Relative) {}
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