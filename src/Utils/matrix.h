#pragma once

#include "logger.h"

template<typename T, uint16_t width, uint16_t heigth>
class Matrix
{
public:
	// Constructors
	Matrix() {
		memset(m_Values, 0, width * heigth * sizeof(T));
	}
	Matrix(const T* data) {
		memcpy(m_Values, data, width * heigth * sizeof(T));
	}
	Matrix(const std::initializer_list<T>& in) {
		RENDERER_ASSERT(in.size() == width * heigth, "Must have the correct dimensions!");
		memcpy(m_Values, in.begin(), width * heigth * sizeof(T));
	}
	Matrix(const std::array<T, width* heigth>& in) {
		memcpy(m_Values, in.data(), width * heigth * sizeof(T));
	}
	// Copy constructor
	Matrix(const Matrix<T, width, heigth>& other) {
		memcpy(m_Values, other.m_Values, width * heigth * sizeof(T));
	}
	// Move constructor
	Matrix(Matrix<T, width, heigth>&& other) noexcept {
		memcpy(m_Values, other.m_Values, width * heigth * sizeof(T));
	}
	// Copy assignment operator
	Matrix<T, width, heigth>& operator=(const Matrix<T, width, heigth>& other) {
		if (this != &other) {
			memcpy(m_Values, other.m_Values, width * heigth * sizeof(T));
		}
		return *this;
	}
	// Move assignment operator
	Matrix<T, width, heigth>& operator=(Matrix<T, width, heigth>&& other) noexcept {
		if (this != &other) {
			memcpy(m_Values, other.m_Values, width * heigth * sizeof(T));
		}
		return *this;
	}
	// Destructor
	~Matrix() = default;

	inline constexpr uint16_t GetWidth() const { return width; }
	inline constexpr uint16_t GetHeigth() const { return heigth; }
	inline constexpr bool IsSquare() const { return width == heigth; }

	inline constexpr uint32_t GetElemCount() const { return width * heigth; }
	inline T* GetData() { return m_Values; }
	inline const T* GetData() const { return m_Values; }

	inline T& operator[](size_t i) { return m_Values[i]; }
	inline const T& operator[](size_t i) const { return m_Values[i]; }


	T& at(size_t r, size_t c) {
		//static_assert(r < heigth && c < width);
		return m_Values[c * heigth + r];
	}
	const T& at(size_t r, size_t c) const {
		//static_assert(r < heigth && c < width);
		return m_Values[c * heigth + r];
	}

	// Mathematical operators
	// +
	Matrix<T, width, heigth> operator+(const Matrix<T, width, heigth>& other) const {
		T data[width * heigth] = { 0 };
		for (size_t i = 0; i < width * heigth; i++) {
			data[i] = m_Values[i] + other.m_Values[i];
		}
		return Matrix<T, width, heigth>(data);
	}
	// Unary negation
	Matrix<T, width, heigth> operator-() const {
		T data[width * heigth] = { 0 };
		for (size_t i = 0; i < width * heigth; i++) {
			data[i] = -m_Values[i];
		}
		return Matrix<T, width, heigth>(data);
	}
	// -
	Matrix<T, width, heigth> operator-(const Matrix<T, width, heigth>& other) const {
		T data[width * heigth] = { 0 };
		for (size_t i = 0; i < width * heigth; i++) {
			data[i] = m_Values[i] - other.m_Values[i];
		}
		return Matrix<T, width, heigth>(data);
	}
	// * Matrix
	template<uint16_t width2>
	Matrix<T, width2, heigth> operator*(const Matrix<T, width2, width>& other) const {
		constexpr uint16_t heigth2 = width;
		const T* o_Values = other.GetData();
		T data[width2 * heigth] = { 0 };
		for (size_t r = 0; r < heigth; r++) {
			for (size_t c = 0; c < width2; c++) {
				//Loop this row in this and this column in other
				for (size_t i = 0; i < width; i++) {
					data[c * heigth + r] += m_Values[i * heigth + r] * o_Values[c * heigth2 + i];
				}
			}
		}
		return Matrix<T, width2, heigth>(data);
	}
	// * scalar
	Matrix<T, width, heigth> operator*(T scalar) const {
		Matrix<T, width, heigth> ret = *this;
		for (size_t i = 0; i < width * heigth; i++) {
			ret[i] *= scalar;
		}
		return ret;
	}

	// length (defined for all, not just vectors)
	float length() const {
		float sqSum = 0.0f;
		for (size_t i = 0; i < width * heigth; i++) {
			sqSum += m_Values[i] * m_Values[i];
		}
		return std::sqrtf(sqSum);
	}
	// square of length (defined for all, not just vectors)
	float lengthSqr() const {
		float sqSum = 0.0f;
		for (size_t i = 0; i < width * heigth; i++) {
			sqSum += m_Values[i] * m_Values[i];
		}
		return sqSum;
	}

	// A normalized vec version (defined for all, not just vectors)
	Matrix<T, width, heigth> normalized() const {
		const float coef = 1.f / this->length();
		return (*this) * coef;
	}

	// Inversion
	Matrix<T, width, heigth> Invert() const {
		static_assert(width == heigth);
		Matrix<T, width, heigth> ret = Identity();
		Matrix<T, width, heigth> copy = (*this);
		//Gaussian elimination
		for (size_t r = 0; r < heigth; r++) {
			//Normalizing the row
			T divider = copy.at(r, r);
			if (divider == static_cast<T>(0)) {
				//We must swap some rows to get a non-zero value here
				//First we find the desired row
				size_t swap_r = r + 1;
				RENDERER_ASSERT(swap_r < width, "Problem with finding non-zero values");
				while (copy.at(swap_r, r) == static_cast<T>(0)) {
					swap_r += 1;
				}
				//And now we swap them
				for (size_t c = 0; c < width; c++) {
					T temp_copy = copy.at(r, c);
					T temp_ret = ret.at(r, c);
					copy.at(r, c) = copy.at(swap_r, c);
					ret.at(r, c) = ret.at(swap_r, c);
					copy.at(swap_r, c) = temp_copy;
					ret.at(swap_r, c) = temp_ret;
				}
				divider = copy.at(r, r);
				RENDERER_ASSERT(divider != static_cast<T>(0), "Zero divider");
			}
			for (size_t c = 0; c < width; c++) {
				copy.at(r, c) = copy.at(r, c) / divider;
				ret.at(r, c) = ret.at(r, c) / divider;
			}
			//Zeroing the other rows in the column
			for (size_t r_inner = 0; r_inner < heigth; r_inner++) {
				if (r_inner != r) {
					T multiplier = copy.at(r_inner, r);
					for (size_t c = 0; c < width; c++) {
						copy.at(r_inner, c) = copy.at(r_inner, c) - multiplier * copy.at(r, c);
						ret.at(r_inner, c) = ret.at(r_inner, c) - multiplier * ret.at(r, c);
					}
				}
			}
		}
		return ret;
	}

	

	//friend Matrix<T, width, heigth>& operator*(const Matrix<T, width, heigth>& m, T scalar);

	static inline Matrix<T, width, width> Identity() {
		static_assert(width == heigth);
		Matrix<T, width, width> res;
		for (size_t i = 0; i < width; i++) {
			res[i * width + i] = static_cast<T>(1);
		}
		return res;
	}

private:
	T m_Values[width * heigth];
};

//template<typename T, uint16_t width, uint16_t heigth>
//Matrix<T, width, heigth>& operator*(const Matrix<T, width, heigth>& m, T scalar) {
//	Matrix<T, width, heigth> ret = m;
//	for (size_t i = 0; i < width * heigth; i++) {
//		ret[i] *= scalar;
//	}
//	return ret;
//}

template<typename T, uint16_t width, uint16_t heigth>
std::ostream& operator<<(std::ostream& os, const Matrix<T, width, heigth>& mat) {
	for (size_t r = 0; r < heigth; r++) {
		for (size_t c = 0; c < width; c++) {
			os << mat.at(r, c) << (c < width - 1 ? ", " : "\n");
		}
	}
	return os;
}

typedef Matrix<float, 2, 2> Mat2;
typedef Matrix<float, 3, 3> Mat3;
typedef Matrix<float, 4, 4> Mat4;

typedef Matrix<float, 1, 2> Vec2;
typedef Matrix<float, 1, 3> Vec3;
typedef Matrix<float, 1, 4> Vec4;

//inline float& r(Vec4& v4) { return v4.GetData()[0]; }
//inline float& g(Vec4& v4) { return v4.GetData()[1]; }
//inline float& b(Vec4& v4) { return v4.GetData()[2]; }
//inline float& a(Vec4& v4) { return v4.GetData()[3]; }


namespace MatrixUtils {
	Mat4 CreateOrtho(float left, float right, float bottom, float top);

	Mat4 CreateOrtho(int32_t left, int32_t right, int32_t bottom, int32_t top);

	Mat4 Translate(const Mat4& m, const Vec3& v);

	Mat4 Rotate(const Mat4& m, const float angle, const Vec3 axis);

	Mat4 CreateTr(float xT, float yT);

	Vec2 MakeVec2f(float x, float y);
}