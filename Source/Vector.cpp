#include "Renderer.h"

using namespace Renderer;
using namespace Renderer::Math;

template<typename T, Size SIZE>
Vector<T, SIZE>::Vector()
{ 
	std::fill(mData.begin(), mData.end(), static_cast<T>(0));
}

template<typename T, Size SIZE>
Vector<T, SIZE>::Vector(const T value)
{
	std::fill(mData.begin(), mData.end(), value);
}

template<typename T, Size SIZE>
Vector<T, SIZE>::Vector(const T* value)
{
	std::copy_n(value, SIZE, mData.begin());
}

template<typename T, Size SIZE>
Vector<T, SIZE>::Vector(std::array<T, SIZE>& data) :
	mData(std::move(data))
{
}

template<typename T, Size SIZE>
Vector<T, SIZE>::Vector(const std::initializer_list<T>& data)
{
	if (SIZE != data.size())
		throw std::logic_error("Intializer list length not the same as vector size.");

	for (Size i = 0; i < SIZE; ++i)
	{
		mData[i] = *(data.begin() + i);
	}
}

template<typename T, Size SIZE>
T Vector<T, SIZE>::Length() const
{
	T sum = 0.0;
	for (const auto& value : mData)
	{
		sum += value * value;
	}
	return std::sqrt(sum);
}

template<typename T, Size SIZE>
void Vector<T, SIZE>::Normalize()
{
	double length = Length();
	std::for_each(mData.begin(), mData.end(), [&](auto& index) { index /= static_cast<T>(length); });
}

template<typename T, Size SIZE>
Vector<T, SIZE> Vector<T, SIZE>::Normalized() const
{
	auto normalized = *this;
	normalized.Normalize();
	return normalized;
}

template<typename T, Size SIZE>
void Vector<T, SIZE>::Clamp(const Size index, const T a, const T b)
{
	mData[index] = Renderer::Math::Clamp<T>(mData[index], a, b);
}

template<typename T, Size SIZE>
void Vector<T, SIZE>::Clamp(const T a, const T b)
{
	std::for_each(mData.begin(), mData.end(), [&](auto& v) { v = Renderer::Math::Clamp<T>(v, a, b); });
}

template<typename T, Size SIZE>
T Vector<T, SIZE>::DotProduct(const Vector<T, SIZE>& v) const
{
	T sum = static_cast<T>(0);
	for (Size i = 0; i < mData.size(); ++i)
		sum += mData[i] * v[i];
	return sum;
}

template<typename T, Size SIZE>
T Vector<T, SIZE>::Distance(const Vector<T, SIZE>& v) const
{
	return (v - *this).Length();
}

template<typename T, Size SIZE>
void Vector<T, SIZE>::SetNaNsOrINFs(const T value, const bool setNaNs, bool setINFs)
{
	for (auto& v : mData)
	{
		if (std::isnan(v) && setNaNs)
			v = value;
		if (std::isinf(v) && setINFs)
			v = value;
	}
}

template<typename T, Size SIZE>
void Vector<T, SIZE>::Pow(const T exponent)
{
	std::for_each(mData.begin(), mData.end(), [&](auto& v) { v = std::pow(v, exponent); });
}

template<typename T, Size SIZE>
Vector<T, SIZE> Vector<T, SIZE>::Mix(const Vector<T, SIZE>& a, const Vector<T, SIZE>& b, const T &amount)
{
	Vector<T, SIZE> result;
	for (Size i = 0; i < result.Data().size(); ++i)
	{
		result[i] = Math::mix<T>(a[i], b[i], amount);
	}
	return result;
}

template<typename T, Size SIZE>
Vector<T, SIZE> Vector<T, SIZE>::Min(const Vector<T, SIZE>& a, const Vector<T, SIZE>& b)
{
	Vector<T, SIZE> result;
	for (Size i = 0; i < result.Data().size(); ++i)
	{
		result[i] = std::min(a[i], b[i]);
	}
	return result;
}

template<typename T, Size SIZE>
Vector<T, SIZE> Vector<T, SIZE>::Max(const Vector<T, SIZE>& a, const Vector<T, SIZE>& b)
{
	Vector<T, SIZE> result;
	for (Size i = 0; i < result.Data().size(); ++i)
	{
		result[i] = std::max(a[i], b[i]);
	}
	return result;
}

template<typename T, Size SIZE>
Vector<T, SIZE> Vector<T, SIZE>::MatrixMultiply(const Matrix<T>& matrix) const
{
	if (matrix.Rows() != mData.size())
		throw std::logic_error("Matrix rows must match vector size.");

	Matrix<T> VtoM(mData.size(), 1u);
	for (Size i = 0; i < matrix.Rows(); ++i)
		VtoM[i] = mData[i];

	const auto result = matrix.Multiply(VtoM);

	Vector<T, SIZE> MtoV;
	for (Size i = 0; i < result.Rows(); ++i)
		MtoV[i] = result[i];

	return MtoV;
}

template<typename T, Size SIZE>
Vector<T, SIZE> Vector<T, SIZE>::CrossProduct(const Vector<T, SIZE>& other) const
{
	if (mData.size() != 3)
		throw std::logic_error("Incorrect number of arguments.");

	auto result = Vector<T, SIZE>();
	result[0] = mData[1] * other[2] - mData[2] * other[1];
	result[1] = mData[2] * other[0] - mData[0] * other[2];
	result[2] = mData[0] * other[1] - mData[1] * other[0];
	return result;
}

template<typename T, Size SIZE>
T Vector<T, SIZE>::AngleBetween(const Vector<T, SIZE>& other) const
{
	if (mData.size() != 3)
		throw std::logic_error("Incorrect number of arguments.");

	return static_cast<T>(0);
}

template class Vector<float, 2>;
template class Vector<float, 3>;