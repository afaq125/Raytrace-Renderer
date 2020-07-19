#pragma once

namespace Renderer
{
	using Size = std::size_t;

	namespace Math
	{
		const double PI = 3.1427;
		const double Infinity = std::numeric_limits<double>::infinity();

		template <typename T>
		T Clamp(const T v, const T a, const T b)
		{
			return std::min(std::max(v, std::min(a, b)), std::max(a, b));
		}
	}
}