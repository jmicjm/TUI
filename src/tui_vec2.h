#pragma once
#include <type_traits>

namespace tui
{
	template <typename T>
	struct vec2
	{
		T x, y;
		vec2() : vec2(0, 0) {}
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

		template<typename Y>
		vec2<T>& operator*=(Y r)
		{
			x *= r;
			y *= r;
			return *this;
		}
		template<typename Y>
		vec2<T>& operator/=(Y r)
		{
			x /= r;
			y /= r;
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

	template<typename T, typename Y>
	inline vec2<typename std::common_type<T, Y>::type> operator*(vec2<T> v, Y r)
	{
		return vec2<typename std::common_type<T, Y>::type>(v.x*r, v.y*r);
	}
	template<typename T, typename Y>
	inline vec2<typename std::common_type<T, Y>::type> operator/(vec2<T> v, Y r)
	{
		return vec2<typename std::common_type<T, Y>::type>(v.x/r, v.y/r);
	}

	using vec2i = vec2<int>;
	using vec2f = vec2<float>;
}