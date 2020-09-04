#pragma once

namespace Renderer
{
	using Size = std::size_t;

	namespace Math
	{
		const float PI = 3.1427f;
		const float Infinity = std::numeric_limits<float>::infinity();
		const float NaN = std::numeric_limits<float>::quiet_NaN();

		template <typename T>
		T Clamp(const T v, const T a, const T b)
		{
			return std::min(std::max(v, std::min(a, b)), std::max(a, b));
		}

		template <typename T>
		T mix(const T &a, const T &b, const T &mix)
		{
			return b * mix + a * (static_cast<T>(1) - mix);
		}
	}
}