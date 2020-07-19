#pragma once

#define CUSTOM_OPERATOR_VECTOR(f) \
	auto result = *this; \
	std::transform(this->mData.begin(), this->mData.end(), rhs.mData.begin(), result.mData.begin(), f); \
	return result; 

#define CUSTOM_OPERATOR_VECTOR_T(f) \
	auto result = *this; \
	std::transform(this->mData.begin(), this->mData.end(), result.mData.begin(), std::bind2nd(f, rhs)); \
	return result; 

#define CUSTOM_OPERATOR_VECTOR_EQUALS(f) \
	std::transform(rhs.mData.begin(), rhs.mData.end(), this->mData.begin(), this->mData.begin(), f); \
	return *this;

#define CUSTOM_OPERATOR_VECTOR_T_EQUALS(f) \
	std::transform(this->mData.begin(), this->mData.end(), this->mData.begin(), std::bind2nd(f, rhs)); \
	return *this;

namespace Renderer
{
	namespace Math
	{
		template<typename T>
		class Matrix;

		template <typename T, Size SIZE>
		class Vector
		{
		public:
			Vector();
			Vector(const T value);
			Vector(const T* data);
			Vector(std::array<T, SIZE>& data);
			Vector(const std::initializer_list<T>& data);
			~Vector() {}

			Vector(const Vector &rhs) = default;
			Vector(Vector &&rhs) = default;
			Vector<T, SIZE>& operator=(const Vector<T, SIZE>& vector) = default;

			// Accessors
			T& operator[] (Size i) { return mData[i]; }
			const T& operator[] (Size i) const { return mData[i]; }

			T Length() const;
			void Normalize();
			Vector<T, SIZE> Normalized() const;
			void Clamp(const Size index, const T a, const T b);
			void Clamp(const T a, const T b);
			T DotProduct(const Vector<T, SIZE>& v) const;
			T Distance(const Vector<T, SIZE>& v) const;
			void SetNaNsOrINFs(const T value, const bool setNaNs = true, bool setINFs = true);

			// Size dependant functions.
			Vector<T, SIZE> MatrixMultiply(const Matrix<T>& matrix) const;
			Vector<T, SIZE> CrossProduct(const Vector<T, SIZE>& other) const;
			T AngleBetween(const Vector<T, SIZE>& other) const;

			// Operators
			Vector<T, SIZE> operator* (const Vector<T, SIZE>& rhs) const { CUSTOM_OPERATOR_VECTOR(std::multiplies<T>()) }
			Vector<T, SIZE> operator+ (const Vector<T, SIZE>& rhs) const { CUSTOM_OPERATOR_VECTOR(std::plus<T>()) }
			Vector<T, SIZE> operator- (const Vector<T, SIZE>& rhs) const { CUSTOM_OPERATOR_VECTOR(std::minus<T>()) }
			Vector<T, SIZE> operator/ (const Vector<T, SIZE>& rhs) const { CUSTOM_OPERATOR_VECTOR(std::divides<T>()) }

			Vector<T, SIZE>& operator*= (const Vector<T, SIZE>& rhs) { CUSTOM_OPERATOR_VECTOR_EQUALS(std::multiplies<T>()) }
			Vector<T, SIZE>& operator+= (const Vector<T, SIZE>& rhs) { CUSTOM_OPERATOR_VECTOR_EQUALS(std::plus<T>()) }
			Vector<T, SIZE>& operator-= (const Vector<T, SIZE>& rhs) { CUSTOM_OPERATOR_VECTOR_EQUALS(std::minus<T>()) }
			Vector<T, SIZE>& operator/= (const Vector<T, SIZE>& rhs) { CUSTOM_OPERATOR_VECTOR_EQUALS(std::divides<T>()) }

			Vector<T, SIZE> operator* (const T& rhs) { CUSTOM_OPERATOR_VECTOR_T(std::multiplies<T>()) }
			Vector<T, SIZE> operator+ (const T& rhs) { CUSTOM_OPERATOR_VECTOR_T(std::plus<T>()) }
			Vector<T, SIZE> operator- (const T& rhs) { CUSTOM_OPERATOR_VECTOR_T(std::minus<T>()) }
			Vector<T, SIZE> operator/ (const T& rhs) { CUSTOM_OPERATOR_VECTOR_T(std::divides<T>()) }

			Vector<T, SIZE>& operator*= (const T& rhs) { CUSTOM_OPERATOR_VECTOR_T_EQUALS(std::multiplies<T>()) }
			Vector<T, SIZE>& operator+= (const T& rhs) { CUSTOM_OPERATOR_VECTOR_T_EQUALS(std::plus<T>()) }
			Vector<T, SIZE>& operator-= (const T& rhs) { CUSTOM_OPERATOR_VECTOR_T_EQUALS(std::minus<T>()) }
			Vector<T, SIZE>& operator/= (const T& rhs) { CUSTOM_OPERATOR_VECTOR_T_EQUALS(std::divides<T>()) }

		protected:
			std::array<T, SIZE> mData;
		};

		using Vector2 = Vector<double, 2>;
		using Vector3 = Vector<double, 3>;

		const Vector3 X_AXIS = { 1.0, 0.0, 0.0 };
		const Vector3 Y_AXIS = { 0.0, 1.0, 0.0 };
		const Vector3 Z_AXIS = { 0.0, 0.0, 1.0 };
	}
}