#pragma once

#define CUSTOM_OPERATOR_MATRIX(f) \
	auto result = *this; \
	std::transform(this->mData.begin(), this->mData.end(), rhs.mData.begin(), result.mData.begin(), f); \
	return result; 

#define CUSTOM_OPERATOR_MATRIX_T(f) \
	auto result = *this; \
	std::transform(this->mData.begin(), this->mData.end(), result.mData.begin(), std::bind2nd(f, rhs)); \
	return result; 

#define CUSTOM_OPERATOR_MATRIX_EQUALS(f) \
	std::transform(rhs.mData.begin(), rhs.mData.end(), this->mData.begin(), this->mData.begin(), f); \
	return *this;

#define CUSTOM_OPERATOR_MATRIX_T_EQUALS(f) \
	std::transform(this->mData.begin(), this->mData.end(), this->mData.begin(), std::bind2nd(f, rhs)); \
	return *this;

namespace Renderer
{
	namespace Math
	{
		template <typename T>
		class Matrix
		{
		public:
			using Coordinate = std::pair<Size, Size>;

			// Constructors
			Matrix();
			Matrix(const Size rows, const Size columns);
			Matrix(const T value, const Size rows, const Size columns);
			Matrix(std::vector<T>&& data, const Size rows, const Size columns);
			Matrix(const std::initializer_list<std::initializer_list<T>>& mat);

			Matrix(const Matrix &rhs) = default;
			Matrix(Matrix &&rhs) = default;
			Matrix<T>& operator=(const Matrix<T>& rhs) = default;
			~Matrix() {}

			// Matrix functions
			void Identity();
			void Transpose();
			Matrix<T> Minors(const Matrix<Size>& mask = Matrix<Size>(0, 0)) const;
			void Cofactor();
			Matrix<T> Cofactors() const;
			T Determinant() const;
			void Inverse();
			static void Laplace(
				const Matrix<T>& matrix,
				const Size index,
				const Size areaLimit,
				Matrix<T>& result);
			T Product(const Matrix<T>& matrix) const;
			Matrix<T> Multiply(const Matrix<T>& matrix) const;

			Matrix<T> Convolution2D(const Matrix<T>& kernel) const;
			std::pair<Matrix<T>, Matrix<Size>> Neighbours(const Coordinate& index, const Size distance = 1) const;

			// Construction
			static Matrix<T> Arrange(const Size rows, const Size columns);

			// Accessors
			T& operator[] (Size i) { return mData[i]; }
			const T& operator[] (Size i) const { return mData[i]; }

			inline Size Area() const { return Rows() * Columns(); }
			inline Size Rows() const { return mRows; }
			inline Size Columns() const { return mColumns; }
			inline Size RowNumberFromIndex(const Size index) const { return index == 0 ? 0 : index / Columns(); }
			inline Size ColumnNumberFromIndex(const Size index) const { return index == 0 ? 0 : index % Columns(); }
			inline Coordinate CoordinateFromIndex(const Size index) const { return std::make_pair(RowNumberFromIndex(index), ColumnNumberFromIndex(index)); }
			inline bool IsSqaure() const { return Rows() == Columns(); }

			inline T Get(const Size r, const Size c) const { return mData[c + (r * Columns())]; }
			inline void Set(const Size x, const Size y, const T value) { mData[y + (x * Rows())] = value; }
			inline const std::vector<T>& Data() const { return mData; }

			std::vector<T> GetRow(const Size row) const;
			std::vector<T> GetColumn(const Size column) const;
			void AddRow(const T value);
			void AddColumn(const T value);

			// Operators
			Matrix<T> operator* (const Matrix<T>& rhs) const { CUSTOM_OPERATOR_MATRIX(std::multiplies<T>()) }
			Matrix<T> operator+ (const Matrix<T>& rhs) const { CUSTOM_OPERATOR_MATRIX(std::plus<T>()) }
			Matrix<T> operator- (const Matrix<T>& rhs) const { CUSTOM_OPERATOR_MATRIX(std::minus<T>()) }
			Matrix<T> operator/ (const Matrix<T>& rhs) const { CUSTOM_OPERATOR_MATRIX(std::divides<T>()) }

			Matrix<T>& operator*= (const Matrix<T>& rhs) { CUSTOM_OPERATOR_MATRIX_EQUALS(std::multiplies<T>()) }
			Matrix<T>& operator+= (const Matrix<T>& rhs) { CUSTOM_OPERATOR_MATRIX_EQUALS(std::plus<T>()) }
			Matrix<T>& operator-= (const Matrix<T>& rhs) { CUSTOM_OPERATOR_MATRIX_EQUALS(std::minus<T>()) }
			Matrix<T>& operator/= (const Matrix<T>& rhs) { CUSTOM_OPERATOR_MATRIX_EQUALS(std::divides<T>()) }

			Matrix<T> operator* (const T& rhs) { CUSTOM_OPERATOR_MATRIX_T(std::multiplies<T>()) }
			Matrix<T> operator+ (const T& rhs) { CUSTOM_OPERATOR_MATRIX_T(std::plus<T>()) }
			Matrix<T> operator- (const T& rhs) { CUSTOM_OPERATOR_MATRIX_T(std::minus<T>()) }
			Matrix<T> operator/ (const T& rhs) { CUSTOM_OPERATOR_MATRIX_T(std::divides<T>()) }

			Matrix<T>& operator*= (const T& rhs) { CUSTOM_OPERATOR_MATRIX_T_EQUALS(std::multiplies<T>()) }
			Matrix<T>& operator+= (const T& rhs) { CUSTOM_OPERATOR_MATRIX_T_EQUALS(std::plus<T>()) }
			Matrix<T>& operator-= (const T& rhs) { CUSTOM_OPERATOR_MATRIX_T_EQUALS(std::minus<T>()) }
			Matrix<T>& operator/= (const T& rhs) { CUSTOM_OPERATOR_MATRIX_T_EQUALS(std::divides<T>()) }

			// Debugging
			std::string Print() const;

		private:
			Size mRows, mColumns;
			std::vector<T> mData;
		};

		template <typename T, Size ROWS, Size COLUMNS>
		class MatrixT : public Matrix<T>
		{
		public:
			MatrixT() : Matrix<T>(ROWS, COLUMNS) {}
			MatrixT(const T value) : Matrix(value, ROWS, COLUMNS) {}
			MatrixT(std::vector<T>&& data) : Matrix(data, ROWS, COLUMNS) {}
			MatrixT(const std::initializer_list<std::initializer_list<T>>& mat) : Matrix(mat)
			{
				Size rows = mat.size();
				Size columns = (*mat.begin()).size();
				if (rows * columns != ROWS * COLUMNS)
					throw std::logic_error("Incorrect number of arguments.");
			}
			~MatrixT() {}
		};

		using Matrix3 = MatrixT<double, 3, 3>;
		using Matrix4 = MatrixT<double, 4, 4>;
	}
}