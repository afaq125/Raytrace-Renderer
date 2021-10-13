#pragma once

namespace Renderer
{
	using Size = std::size_t;

	namespace Math
	{
		constexpr float PI = 3.1427f;
        constexpr float PI2 = PI * 2.0f;
		constexpr float Infinity = std::numeric_limits<float>::infinity();
		constexpr float NaN = std::numeric_limits<float>::quiet_NaN();

		template <typename T>
		T Clamp(const T v, const T a, const T b)
		{
			return std::min(std::max(v, std::min(a, b)), std::max(a, b));
		}

		template <typename T>
		T Mix(const T &a, const T &b, const T &mix)
		{
			return b * mix + a * (static_cast<T>(1) - mix);
		}

		template<typename T>
		bool IsNearValue(const T a, const T b, const T tolerance = static_cast<T>(0.0001))
		{
			return std::abs(std::abs(a) - std::abs(b)) <= tolerance;
		}
	}
}