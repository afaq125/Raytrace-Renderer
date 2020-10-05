#include "Renderer.h"

using namespace Renderer;
using namespace Renderer::Math;

// Constructors

template<typename T>
Matrix<T>::Matrix() :
	mRows(0),
	mColumns(0)
{
}

template<typename T>
Matrix<T>::Matrix(const Size rows, const Size columns) :
	mRows(rows),
	mColumns(columns)
{
	mData.resize(rows * columns);
}

template<typename T>
Matrix<T>::Matrix(const T value, const Size rows, const Size columns) :
	Matrix<T>::Matrix(rows, columns)
{
	std::fill(mData.begin(), mData.end(), value);
}

template<typename T>
Matrix<T>::Matrix(std::vector<T>&& data, const Size rows, const Size columns) :
	mRows(rows),
	mColumns(columns)
{
	if (Area() != data.size())
		std::logic_error("Row and Columns area does not match data size.");
	mData = std::move(data);
}

template<typename T>
Matrix<T>::Matrix(const std::initializer_list<std::initializer_list<T>>& mat)
{
	mRows = mat.size();
	mColumns = (*mat.begin()).size();
	for (const auto& r : mat)
	{
		for (const auto& c : r)
		{
			mData.push_back(c);
		}
	}
}

// Matrix functions

template<typename T>
void Matrix<T>::Identity()
{	
	if (!IsSqaure())
		return;

	std::fill(mData.begin(), mData.end(), static_cast<T>(0));
	Size offset = 0;
	for (Size i = 0; i < Area(); i += Rows())
	{
		mData[offset + i] = 1;
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
			const Size tIndex = (Rows() * c) + r;
			transposed[index] = mData[tIndex];
		}
	}
	mData = transposed;
	std::swap(mRows, mColumns);
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
		mData[i] = i % 2 ? mData[i] * static_cast<T>(-1) : mData[i];
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
		throw std::logic_error("Matrix is not square.");

	std::vector<T> determinants(Columns());
	for (Size i = 0; i < Columns(); ++i)
	{
		Matrix<T> expanded(0,0);
		Laplace(*this, i, 4, expanded);
		determinants[i] = mData[i] * ((expanded[0] * expanded[3]) - (expanded[1] * expanded[2]));
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
	mData = Minors().Data();
	Cofactor();

	// Determinant can can be calculated faster by reusing minors.
	T value = static_cast<T>(1) / determinant;
	std::for_each(mData.begin(), mData.end(), [&](auto &i) { i *= value; });
}

template<typename T>
Matrix<T> Matrix<T>::Inversed() const
{
	auto inversed = *this;
	inversed.Inverse();
	return inversed;
}

template<typename T>
void Matrix<T>::Laplace(
	const Matrix<T>& matrix,
	const Size index,
	const Size area,
	Matrix<T>& result)
{
	if (!matrix.IsSqaure() || matrix.Area() < area)
		throw std::logic_error("Matrix is not square or already smaller than final area.");

	if (matrix.Area() != area)
	{
		Matrix<T> expanded(matrix.Rows() - 1, matrix.Columns() - 1);
		const Size rIndex = matrix.RowNumberFromIndex(index);
		const Size cIndex = matrix.ColumnNumberFromIndex(index);

		Size eIndex = 0;
		for (Size r = 0; r < matrix.Rows(); ++r)
		{
			for (Size c = 0; c < matrix.Columns(); ++c)
			{
				const Size index = matrix.Columns() * r + c;
				if (r == rIndex || c == cIndex)
					continue;
				expanded[eIndex] = matrix[index];
				++eIndex;
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
T Matrix<T>::Product(const Matrix<T>& matrix) const
{
	T sum = static_cast<T>(0);
	for (Size i = 0; i < Area(); ++i)
		sum += mData[i] * matrix[i];
	return sum;
}

template<typename T>
T Matrix<T>::Sum() const
{
	T sum = static_cast<T>(0);
	for (const auto& value : mData)
		sum += value;
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
	if (Columns() != matrix.Rows() )
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
				Size thisOffset = Columns() * r + i;
				Size otherOffset = matrix.Rows() * c + i;
				sum += mData[thisOffset] * matrix[otherOffset];
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
	const int& row = static_cast<int>(index.first);
	const int& column = static_cast<int>(index.second);

	auto validate = [&](const int& value, const int& max) -> int
	{
		if (value < 0)
			return 0;
		if (value > max)
			return max;
		return value;
	};
	const int minR = validate(row - static_cast<int>(distance), static_cast<int>(Area()));
	const int minC = validate(column - static_cast<int>(distance), static_cast<int>(Area()));
	const int maxR = validate(row + static_cast<int>(distance), static_cast<int>(Rows()) - 1);
	const int maxC = validate(column + static_cast<int>(distance), static_cast<int>(Columns()) - 1);
	const int rows = (maxR - minR) + 1;
	const int columns = (maxC - minC) + 1;
	const int area = rows * columns;
	const int start = (minR * (static_cast<int>(Area()) / static_cast<int>(Rows()))) + minC;

	const int realRC = (static_cast<int>(distance) * 2) + 1;
	const int realRStart = maxR > std::ceil(double(rows) / 2.0) ? 0 : realRC - rows;
	const int realCStart = maxC > std::ceil(double(columns) / 2.0) ? 0 : realRC - columns;
	const int realStart = realRStart * realRC + realCStart;

	Matrix<T> neighboursV(static_cast<T>(0), realRC, realRC);
	Matrix<Size> neighboursI(0, rows, columns);
	Size offset = start;
	Size offsetV = realStart;
	for (Size r = 0; r < rows; ++r)
	{
		for (Size c = 0; c < columns; ++c)
		{
			const Size index = columns * r + c;
			neighboursV[offsetV + c] = mData[offset + c];
			neighboursI[index] = offset + c;
		}
		offset += Columns();
		offsetV += realRC;
	}

	return { neighboursV, neighboursI };
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
	return { mData.begin() + (row * Rows()), mData.begin() + (row * Rows()) + Rows() }; 
}

template<typename T>
std::vector<T> Matrix<T>::GetColumn(const Size column) const 
{ 
	return mData; 
}

template<typename T>
void Matrix<T>::AddRow(const T value) 
{ 
	mData.resize(mData.size() + Columns(), value); ++mRows; 
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
		matrix << mData[i];
		matrix << ", ";
	}
	matrix << "\n";
	return matrix.str();
}

template class Matrix<int>;
template class Matrix<Size>;
template class Matrix<float>;
template class Matrix<double>;