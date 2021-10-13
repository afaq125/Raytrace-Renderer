#include "Renderer.h"

using namespace Renderer;
using namespace Renderer::Math;

// Constructors

template<typename T>
Matrix<T>::Matrix() :
	m_rows(0),
	m_columns(0)
{
}

template<typename T>
Matrix<T>::Matrix(const Size rows, const Size columns) :
	m_rows(rows),
	m_columns(columns)
{
	m_data.resize(rows * columns);
}

template<typename T>
Matrix<T>::Matrix(const T value, const Size rows, const Size columns) :
	Matrix<T>::Matrix(rows, columns)
{
	std::fill(m_data.begin(), m_data.end(), value);
}

template<typename T>
Matrix<T>::Matrix(std::vector<T> data, const Size rows, const Size columns) :
	m_rows(rows),
	m_columns(columns)
{
	if (Area() != data.size())
	{
		throw std::logic_error("Row and Columns area does not match data size.");
	}
	m_data = std::move(data);
}

template<typename T>
Matrix<T>::Matrix(const std::initializer_list<std::initializer_list<T>>& mat)
{
	m_rows = mat.size();
	m_columns = (*mat.begin()).size();
	for (const auto& r : mat)
	{
		for (const auto& c : r)
		{
			m_data.push_back(c);
		}
	}
}

// Matrix functions

template<typename T>
void Matrix<T>::Identity()
{	
	if (!IsSqaure())
	{
		return;
	}

	std::fill(m_data.begin(), m_data.end(), static_cast<T>(0));
	Size offset = 0;
	for (Size i = 0; i < Area(); i += Rows())
	{
		m_data[offset + i] = 1;
		++offset;
	}
}

template<typename T>
void Matrix<T>::Transpose()
{
	std::vector<T> transposed(Area());
	for (Size r = 0; r < Rows(); ++r)
	{
		for (Size c = 0; c < Columns(); ++c)
		{
			const Size index = Columns() * r + c;
			const Size t_index = (Rows() * c) + r;
			transposed[index] = m_data[t_index];
		}
	}
	m_data = transposed;
	std::swap(m_rows, m_columns);
}

template<typename T>
Matrix<T> Matrix<T>::Minors(const Matrix<Size>& mask) const
{
	Matrix<T> result(Rows(), Columns());
	for (Size i = 0; i < Area(); ++i)
	{
		if (mask.Area() == 0 || mask[i] == 1)
		{
			Matrix<T> expanded(0, 0);
			Laplace(*this, i, 4, expanded);
			const T determinant = ((expanded[0] * expanded[3]) - (expanded[1] * expanded[2]));
			result[i] = determinant;
		}
	};
	return result;
}

template<typename T>
void Matrix<T>::Cofactor()
{
	for (Size i = 0; i < Area(); ++i)
	{
		m_data[i] = i % 2 ? m_data[i] * static_cast<T>(-1) : m_data[i];
	}
}

template<typename T>
Matrix<T> Matrix<T>::Cofactors() const
{
	Matrix<T> matrix = Matrix<T>(static_cast<T>(1), Rows(), Columns());
	matrix.Cofactor();
	return matrix;
}

template<typename T>
T Matrix<T>::Determinant() const
{
	if (!IsSqaure())
	{
		throw std::logic_error("Matrix is not square.");
	}

	std::vector<T> determinants(Columns());
	for (Size i = 0; i < Columns(); ++i)
	{
		Matrix<T> expanded(0,0);
		Laplace(*this, i, 4, expanded);
		determinants[i] = m_data[i] * ((expanded[0] * expanded[3]) - (expanded[1] * expanded[2]));
	}

	Size i = 0;
	const T determinant = std::accumulate(determinants.begin(), determinants.end(), static_cast<T>(0), [&](auto a, auto b)
	{ 
		auto v = i % 2 ? a - b : a + b;
		++i;
		return v;
	});

	return determinant;
}

template<typename T>
void Matrix<T>::Inverse()
{
	const T determinant = Determinant();
	Transpose();
	m_data = Minors().Data();
	Cofactor();

	T value = static_cast<T>(1) / determinant;
	for(auto &i : m_data) { i *= value; };
}

template<typename T>
Matrix<T> Matrix<T>::Inversed() const
{
	return GaussElimination();
}

template<typename T>
void Matrix<T>::Laplace(
	const Matrix<T>& matrix,
	const Size index,
	const Size area,
	Matrix<T>& result)
{
	if (!matrix.IsSqaure() || matrix.Area() < area)
	{
		throw std::logic_error("Matrix is not square or already smaller than final area.");
	}

	if (matrix.Area() != area)
	{
		Matrix<T> expanded(matrix.Rows() - 1, matrix.Columns() - 1);
		const Size r_index = matrix.RowNumberFromIndex(index);
		const Size c_index = matrix.ColumnNumberFromIndex(index);

		Size e_index = 0;
		for (Size r = 0; r < matrix.Rows(); ++r)
		{
			for (Size c = 0; c < matrix.Columns(); ++c)
			{
				const Size index = matrix.Columns() * r + c;
				if (r == r_index || c == c_index)
					continue;
				expanded[e_index] = matrix[index];
				++e_index;
			}
		}

		result = expanded;
		Matrix<T>::Laplace(expanded, 0, area, result);
	}

	if (matrix.Area() == area)
	{
		result = matrix;
	}
}

template<typename T>
Matrix<T> Matrix<T>::GaussElimination(const Matrix<T>& solution) const
{
	if (!IsSqaure())
	{
		throw std::logic_error("Matrix is not square.");
	}

	auto coefficient = *this;
	auto result = solution;

	if (result.Area() != Area())
	{
		result = coefficient;
		result.Identity();
	}

	const Size size = coefficient.Columns();
	std::vector<T> pivot_row(size);
	std::vector<T> result_pivot_row(size);

	for (Size i = 0; i < size; ++i)
	{
		const T pivot = coefficient[(size * i) + i] + static_cast<T>(0.000001);
		for (Size c = 0; c < size; ++c)
		{
			const Size offset = (size * i) + c;
			pivot_row[c] = coefficient[offset] / pivot;
			result_pivot_row[c] = result[offset] / pivot;
		}

		for (Size r = 0; r < size; ++r)
		{
			if (r == i)
			{
				continue;
			}

			const Size offset = (size * r) + i;
			const T first = coefficient[offset];

			for (Size c = 0; c < size; ++c)
			{
				const Size index = size * r + c;
				coefficient[index] = coefficient[index] - (first * pivot_row[c]);
				result[index] = result[index] - (first * result_pivot_row[c]);
			}
		}

		for (Size c = 0; c < size; ++c)
		{
			const Size offset = (size * i) + c;
			coefficient[offset] = pivot_row[c];
			result[offset] = result_pivot_row[c];
		}
	}

	return result;
}

template<typename T>
T Matrix<T>::Product(const Matrix<T>& matrix) const
{
	T sum = static_cast<T>(0);
	for (Size i = 0; i < Area(); ++i)
	{
		sum += m_data[i] * matrix[i];
	}
	return sum;
}

template<typename T>
T Matrix<T>::Sum() const
{
	T sum = static_cast<T>(0);
	for (const auto& value : m_data)
	{
		sum += value;
	}
	return sum;
}

template<typename T>
void Matrix<T>::Normalize()
{
	*this = *this * Sum();
}

template<typename T>
Matrix<T> Matrix<T>::Normalized() const
{
	auto result = *this;
	result.Normalize();
	return result;
}

template<typename T>
Matrix<T> Matrix<T>::Multiply(const Matrix<T>& matrix) const
{
	if (Columns() != matrix.Rows())
	{
		throw std::logic_error("Diemnsions do not match for matrix multiplication. This matrix rows must match input columns.");
	}

	auto result = Matrix<T>(Rows(), matrix.Columns());
	for (Size r = 0; r < result.Rows(); ++r)
	{
		for (Size c = 0; c < result.Columns(); ++c)
		{
			const Size index = result.Columns() * r + c;
			T sum = static_cast<T>(0);

			for (Size i = 0; i < Columns(); ++i)
			{
				Size this_offset = Columns() * r + i;
				Size other_offset = matrix.Rows() * c + i;
				sum += m_data[this_offset] * matrix[other_offset];
			}

			result[index] = sum;
		}
	}
	return result;
}

template<typename T>
Matrix<T> Matrix<T>::Convolution2D(const Matrix<T>& kernel) const
{
	if (kernel.Columns() > Columns() || 
		kernel.Rows() > Rows())
	{
		throw std::logic_error("Kernel dimensions are larger than matrix.");
	}

	Matrix<T> result = Matrix<T>(Rows(), Columns());

	auto job = [&](Size i) -> void
	{
		const Size r = RowNumberFromIndex(i);
		const Size c = ColumnNumberFromIndex(i);
		const Size index = Columns() * r + c;
		auto neighbours = Neighbours({ r, c }, 2);
		result[index] = neighbours.first.Product(kernel);
	};

	ThreadPool::Run(job, Area()); 

	return result;
}

template<typename T>
std::pair<Matrix<T>, Matrix<Size>> Matrix<T>::Neighbours(const typename Matrix<T>::Coordinate& index, const Size distance) const
{
	const int row = static_cast<int>(index.first);
	const int column = static_cast<int>(index.second);

	auto validate = [&](const int& value, const int& max) -> int
	{
		if (value < 0)
			return 0;
		if (value > max)
			return max;
		return value;
	};

	const int min_r = validate(row - static_cast<int>(distance), static_cast<int>(Area()));
	const int min_c = validate(column - static_cast<int>(distance), static_cast<int>(Area()));
	const int max_r = validate(row + static_cast<int>(distance), static_cast<int>(Rows()) - 1);
	const int max_c = validate(column + static_cast<int>(distance), static_cast<int>(Columns()) - 1);
	const int rows = (max_r - min_r) + 1;
	const int columns = (max_c - min_c) + 1;
	const int area = rows * columns;
	const int start = (min_r * (static_cast<int>(Area()) / static_cast<int>(Rows()))) + min_c;

	const int real_rc = (static_cast<int>(distance) * 2) + 1;
	const int real_r_start = max_r > std::ceil(double(rows) / 2.0) ? 0 : real_rc - rows;
	const int real_c_start = max_c > std::ceil(double(columns) / 2.0) ? 0 : real_rc - columns;
	const int real_start = real_r_start * real_rc + real_c_start;

	Matrix<T> neighbours_values(static_cast<T>(0), real_rc, real_rc);
	Matrix<Size> neighbours_indicies(0, rows, columns);
	Size offset = start;
	Size offset_v = real_start;
	for (Size r = 0; r < rows; ++r)
	{
		for (Size c = 0; c < columns; ++c)
		{
			const Size index = columns * r + c;
            neighbours_values[offset_v + c] = m_data[offset + c];
            neighbours_indicies[index] = offset + c;
		}
		offset += Columns();
        offset_v += real_rc;
	}

	return { neighbours_values, neighbours_indicies };
}

// Construction

template<typename T>
Matrix<T> Matrix<T>::Arrange(const Size rows, const Size columns)
{
	Matrix<T> matrix(rows, columns);
	for (Size i = 0; i < matrix.Area(); ++i)
	{
		matrix[i] = static_cast<T>(i);
	}
	return matrix;
}

// Modify data size.

template<typename T>
std::vector<T> Matrix<T>::GetRow(const Size row) const 
{ 
	return { m_data.begin() + (row * Rows()), m_data.begin() + (row * Rows()) + Rows() }; 
}

template<typename T>
std::vector<T> Matrix<T>::GetColumn(const Size column) const 
{ 
	return m_data; 
}

template<typename T>
void Matrix<T>::AddRow(const T value) 
{ 
	m_data.resize(m_data.size() + Columns(), value); ++m_rows; 
}

template<typename T>
void Matrix<T>::AddColumn(const T value) 
{
}

// Debugging

template<typename T>
std::string Matrix<T>::Print() const
{
	std::stringstream matrix;
	matrix << "Size: " << Area() << "\n";
	for (int i = 0; i < Area(); ++i)
	{
		if ((i % Columns()) == 0 && i != 0)
		{
			matrix << "\n";
		}
		matrix << m_data[i];
		matrix << ", ";
	}
	matrix << "\n";
	return matrix.str();
}

template class Matrix<int>;
template class Matrix<Size>;
template class Matrix<float>;
template class Matrix<double>;