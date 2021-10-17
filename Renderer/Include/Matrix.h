#pragma once

#define CUSTOM_OPERATOR_MATRIX(op) \
	auto result = *this; \
	const Size count = result.Area(); \
	for (Size i = 0; i < count; ++i) { result.m_data[i] op= rhs.m_data[i]; } \
	return result; 

#define CUSTOM_OPERATOR_MATRIX_T(op) \
	auto result = *this; \
	const Size count = result.Area(); \
	for (Size i = 0; i < count; ++i) { result.m_data[i] op= rhs; } \
	return result; 

#define CUSTOM_OPERATOR_MATRIX_EQUALS(op) \
	const Size count = Area(); \
	for (Size i = 0; i < count; ++i) { this->m_data[i] op= rhs.m_data[i]; } \
	return *this;

#define CUSTOM_OPERATOR_MATRIX_T_EQUALS(op) \
	const Size count = Area(); \
	for (Size i = 0; i < count; ++i) { this->m_data[i] op= rhs; } \
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
			Matrix(std::vector<T> data, const Size rows, const Size columns);
			Matrix(const std::initializer_list<std::initializer_list<T>>& mat);

			Matrix(const Matrix &rhs) = default;
			Matrix(Matrix &&rhs) = default;
			Matrix& operator=(const Matrix& rhs) = default;
			Matrix& operator=(Matrix&& rhs) = default;
			virtual ~Matrix() {}

			// Matrix functions
			void Identity();
			void Transpose();
			Matrix Minors(const Matrix<Size>& mask = Matrix<Size>(0, 0)) const;
			void Cofactor();
			Matrix Cofactors() const;
			T Determinant() const;
			void Inverse();
			Matrix Inversed() const;
			static void Laplace(
				const Matrix& matrix,
				const Size index,
				const Size areaLimit,
				Matrix& result);
			Matrix GaussElimination(const Matrix& solution = Matrix()) const;
			T Product(const Matrix& matrix) const;
			T Sum() const;
			void Normalize();
			Matrix Normalized() const;
			Matrix Multiply(const Matrix& matrix) const;

			Matrix Convolution2D(const Matrix& kernel) const;
			std::pair<Matrix, Matrix<Size>> Neighbours(const Coordinate& index, const Size distance = 1) const;

			// Construction
			static Matrix Arrange(const Size rows, const Size columns);

			// Accessors
			T& operator[] (Size i) { return m_data[i]; }
			const T& operator[] (Size i) const { return m_data[i]; }

			inline Size Area() const { return Rows() * Columns(); }
			inline Size Rows() const { return m_rows; }
			inline Size Columns() const { return m_columns; }
			inline Size RowNumberFromIndex(const Size index) const { return index == 0 ? 0 : index / Columns(); }
			inline Size ColumnNumberFromIndex(const Size index) const { return index == 0 ? 0 : index % Columns(); }
			inline Coordinate CoordinateFromIndex(const Size index) const { return std::make_pair(RowNumberFromIndex(index), ColumnNumberFromIndex(index)); }
			inline bool IsSqaure() const { return Rows() == Columns(); }

			inline T Get(const Size c, const Size r) const { return m_data[(Columns() * r) + c]; }
			inline void Set(const Size c, const Size r, const T value) { m_data[(Columns() * r) + c] = value; }
			inline std::vector<T>& Data() { return m_data; }
            inline const std::vector<T>& Data() const { return m_data; }

			std::vector<T> GetRow(const Size row) const;
			std::vector<T> GetColumn(const Size column) const;
			void AddRow(const T value);
			void AddColumn(const T value);

			// Operators
			Matrix operator* (const Matrix& rhs) const { CUSTOM_OPERATOR_MATRIX(*) }
			Matrix operator+ (const Matrix& rhs) const { CUSTOM_OPERATOR_MATRIX(+) }
			Matrix operator- (const Matrix& rhs) const { CUSTOM_OPERATOR_MATRIX(-) }
			Matrix operator/ (const Matrix& rhs) const { CUSTOM_OPERATOR_MATRIX(/) }

			Matrix& operator*= (const Matrix& rhs) { CUSTOM_OPERATOR_MATRIX_EQUALS(*) }
			Matrix& operator+= (const Matrix& rhs) { CUSTOM_OPERATOR_MATRIX_EQUALS(+) }
			Matrix& operator-= (const Matrix& rhs) { CUSTOM_OPERATOR_MATRIX_EQUALS(-) }
			Matrix& operator/= (const Matrix& rhs) { CUSTOM_OPERATOR_MATRIX_EQUALS(/) }

			Matrix operator* (const T& rhs) { CUSTOM_OPERATOR_MATRIX_T(*) }
			Matrix operator+ (const T& rhs) { CUSTOM_OPERATOR_MATRIX_T(+) }
			Matrix operator- (const T& rhs) { CUSTOM_OPERATOR_MATRIX_T(-) }
			Matrix operator/ (const T& rhs) { CUSTOM_OPERATOR_MATRIX_T(/) }

		    Matrix& operator*= (const T& rhs) { CUSTOM_OPERATOR_MATRIX_T_EQUALS(*) }
			Matrix& operator+= (const T& rhs) { CUSTOM_OPERATOR_MATRIX_T_EQUALS(+) }
			Matrix& operator-= (const T& rhs) { CUSTOM_OPERATOR_MATRIX_T_EQUALS(-) }
			Matrix& operator/= (const T& rhs) { CUSTOM_OPERATOR_MATRIX_T_EQUALS(/) }

			// Debugging
			std::string Print() const;

		private:
			Size m_rows, m_columns;
			std::vector<T> m_data;
		};

		template <typename T, Size SIZE>
		class SquareMatrix : public Matrix<T>
		{
		public:
			SquareMatrix() : Matrix(SIZE, SIZE) {}
			SquareMatrix(const T value) : Matrix(value, SIZE, SIZE) {}
			SquareMatrix(std::vector<T> data) : Matrix(std::move(data), SIZE, SIZE) {}
			SquareMatrix(const std::initializer_list<std::initializer_list<T>>& mat) : Matrix(mat)
			{
				Size rows = mat.size();
				Size columns = (*mat.begin()).size();
				if (rows * columns != SIZE * SIZE)
					throw std::logic_error("Incorrect number of arguments.");
			}
			~SquareMatrix() {}
		};

		using Matrix3 = SquareMatrix<float, 3>;
		using Matrix4 = SquareMatrix<float, 4>;
	}
}