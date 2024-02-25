#include "matrix.h"

Mat4 MatrixUtils::CreateOrtho(float left, float right, float bottom, float top)
{
	float data[16] = {
		2.0f / (right - left), 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f, 0.0f,
		-(right + left) / (right - left), -(top + bottom) / (top - bottom), 0.0f, 0.0f
	};
	return Mat4(data);
}

Mat4 MatrixUtils::CreateOrtho(int32_t left, int32_t right, int32_t bottom, int32_t top)
{
	return CreateOrtho(static_cast<float>(left),
		static_cast<float>(right),
		static_cast<float>(bottom),
		static_cast<float>(top));
}

Mat4 MatrixUtils::Translate(const Mat4& m, const Vec3& v)
{
	Mat4 result(m);
	constexpr uint16_t w = result.GetWidth(), h = result.GetHeigth();
	for (size_t i = 0; i < w; i++) {
		result[i * h + 3] += m[i * h + 0] * v[0] + m[i * h + 1] * v[1] + m[i * h + 2] * v[2];
	}
	return result;
}

Mat4 MatrixUtils::Rotate(const Mat4& m, const float angle, const Vec3 axis)
{
	// Todo, implement rotation
	return m;
}

Mat4 MatrixUtils::CreateTr(float xT, float yT)
{
	float data[16] = {
		1.0, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		  xT,   yT, 0.0f, 1.0f
	};
	return Mat4(data);
}

Vec2 MatrixUtils::MakeVec2f(float x, float y)
{
	float data[2] = { x, y };
	return Vec2(data);
}