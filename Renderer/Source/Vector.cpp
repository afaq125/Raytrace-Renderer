#include "Renderer.h"

using namespace Renderer;
using namespace Renderer::Math;

template<typename T, Size S>
Vector<T, S>::Vector()
{
    m_data = std::vector<T>(S);
}

template<typename T, Size S>
Vector<T, S>::Vector(const T value)
{
    m_data = std::vector<T>(S, value);
}

template<typename T, Size S>
Vector<T, S>::Vector(const T* data)
{
	std::copy_n(data, S, std::back_inserter(m_data));
}

template<typename T, Size S>
Vector<T, S>::Vector(std::vector<T> data) :
	m_data(std::move(data))
{
    if (m_data.size() != S)
    {
        throw std::runtime_error("Invalid size of std::vector.");
    }
}

template<typename T, Size S>
Vector<T, S>::Vector(const std::initializer_list<T>& data)
{
    m_data = std::vector<T>(S);
    std::copy(data.begin(), data.end(), m_data.begin());
}

template<typename T, Size S>
T Vector<T, S>::Length() const
{
	T sum = 0.0;
	for (const auto& value : m_data)
	{
		sum += value * value;
	}
	return std::sqrt(sum);
}

template<typename T, Size S>
void Vector<T, S>::Normalize()
{
	double length = Length();
	std::for_each(m_data.begin(), m_data.end(), [&](auto& index) { index /= static_cast<T>(length); });
}

template<typename T, Size S>
Vector<T, S> Vector<T, S>::Normalized() const
{
	auto normalized = *this;
	normalized.Normalize();
	return normalized;
}

template<typename T, Size S>
void Vector<T, S>::Clamp(const Size index, const T a, const T b)
{
	m_data[index] = Renderer::Math::Clamp<T>(m_data[index], a, b);
}

template<typename T, Size S>
void Vector<T, S>::Clamp(const T a, const T b)
{
	std::for_each(m_data.begin(), m_data.end(), [&](auto& v) { v = Renderer::Math::Clamp<T>(v, a, b); });
}

template<typename T, Size S>
T Vector<T, S>::DotProduct(const Vector<T, S>& v) const
{
	T sum = static_cast<T>(0);
	for (Size i = 0; i < m_data.size(); ++i)
		sum += m_data[i] * v[i];
	return sum;
}

template<typename T, Size S>
T Vector<T, S>::Distance(const Vector<T, S>& v) const
{
	return (v - *this).Length();
}

template<typename T, Size S>
void Vector<T, S>::SetNaNsOrINFs(const T value, const bool setNaNs, bool setINFs)
{
	for (auto& v : m_data)
	{
		if (std::isnan(v) && setNaNs)
			v = value;
		if (std::isinf(v) && setINFs)
			v = value;
	}
}

template<typename T, Size S>
void Vector<T, S>::Pow(const T exponent)
{
	std::for_each(m_data.begin(), m_data.end(), [&](auto& v) { v = std::pow(v, exponent); });
}

template<typename T, Size S>
Vector<T, S> Vector<T, S>::Mix(const Vector<T, S>& a, const Vector<T, S>& b, const T &amount)
{
	Vector<T, S> result;
    for (Size i = 0; i < result.Data().size(); ++i)
	{
		result[i] = Math::Mix<T>(a[i], b[i], amount);
	}
	return result;
}

template<typename T, Size S>
Vector<T, S> Vector<T, S>::Min(const Vector<T, S>& a, const Vector<T, S>& b)
{
    Vector<T, S> result;
	for (Size i = 0; i < result.Data().size(); ++i)
	{
		result[i] = std::min(a[i], b[i]);
	}
	return result;
}

template<typename T, Size S>
Vector<T, S> Vector<T, S>::Max(const Vector<T, S>& a, const Vector<T, S>& b)
{
	Vector<T, S> result;
	for (Size i = 0; i < result.Data().size(); ++i)
	{
		result[i] = std::max(a[i], b[i]);
	}
	return result;
}

template<typename T, Size S>
Vector<T, S> Vector<T, S>::MatrixMultiply(const Matrix<T>& matrix) const
{
	if (matrix.Rows() != m_data.size())
		throw std::logic_error("Matrix rows must match vector size.");

	const Matrix<T> v_to_m(m_data, m_data.size(), 1u);
    Vector<T, S> m_to_v(std::move(matrix.Multiply(v_to_m).Data()));

	return m_to_v;
}

template<typename T, Size S>
Vector<T, S> Vector<T, S>::CrossProduct(const Vector<T, S>& other) const
{
	auto result = Vector<T, S>();
	result[0] = m_data[1] * other[2] - m_data[2] * other[1];
	result[1] = m_data[2] * other[0] - m_data[0] * other[2];
	result[2] = m_data[0] * other[1] - m_data[1] * other[0];
	return result;
}

template class Vector<float, 2>;
template class Vector<float, 3>;
