#pragma once
#include <type_traits>
#include <chrono>
#include <thread>

namespace tui 
{
	template <typename T>
	struct vec2
	{
		T x, y;
		vec2() : vec2(0,0) {}
		vec2(T X, T Y) : x(X), y(Y) {}

		template <typename Y>
		vec2(const vec2<Y>& r)
		{
			x = r.x;
			y = r.y;
		}

		template <typename Y>
		vec2<T>& operator=(vec2<Y> r)
		{
			x = r.x;
			y = r.y;
			return *this;
		}

		template <typename Y>
		bool operator==(vec2<Y> r) const
		{
			return x == r.x && y == r.y;
		}
		template <typename Y>
		bool operator!=(vec2<Y> r) const
		{
			return x != r.x || y != r.y;
		}

		template <typename Y>
		vec2<T>& operator+=(vec2<Y> r)
		{
			x += r.x;
			y += r.y;
			return *this;
		}
		template <typename Y>
		vec2<T>& operator-=(vec2<Y> r)
		{
			x -= r.x;
			y -= r.y;
			return *this;
		}
	};

	template <typename T, typename Y>
	inline vec2<typename std::common_type<T, Y>::type> operator+(vec2<T> l, vec2<Y> r)
	{
		return vec2<typename std::common_type<T, Y>::type>(l.x + r.x, l.y + r.y);
	}
	template <typename T, typename Y>
	inline vec2<typename std::common_type<T, Y>::type> operator-(vec2<T> l, vec2<Y> r)
	{
		return vec2<typename std::common_type<T, Y>::type>(l.x - r.x, l.y - r.y);
	}

	using vec2i = vec2<int>;
	using vec2f = vec2<float>;

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