#include "Renderer.h"

using namespace Renderer;
using namespace Renderer::Math;

void Camera::SetAspectRatio(const float a, const float b)
{
	const float ratio = a / b;
	Height /= ratio;
	Initialise();
}

void Camera::SetPixel(const Size index, const float r, const float g, const float b)
{
	mViewport[0][index] = r;
	mViewport[1][index] = g;
	mViewport[2][index] = b;
}

Vector3 Camera::GetPixelPosition(const float u, const float v) const
{
	const float hWidth = Width / 2.0f;
	const float hHeight = Height / 2.0f;

	const float x = ((Width * u) - hWidth) * PixelSpacing;
	const float z = ((Height * v) - hHeight) * PixelSpacing;

	Vector3 position = { x, -FocalLength, z };
	auto transformed = position.MatrixMultiply(XForm.GetAxis());
	transformed += XForm.GetPosition();

	return transformed;
}

void Camera::LookAt(const Vector3& target, const Vector3& up)
{
	const auto direction = XForm.GetPosition() - target;
	const auto axis = Transform(direction, up, XForm.GetPosition(), true);
	XForm.SetAxis(axis.GetAxis());
}

Vector2 Camera::PixelToUV(const Size index) const
{
	const float column = static_cast<float>(mViewport[0].ColumnNumberFromIndex(index));
	const float row = static_cast<float>(mViewport[0].RowNumberFromIndex(index));
	const float u = column / static_cast<float>(mViewport[0].Columns());
	const float v = row / static_cast<float>(mViewport[0].Rows());
	return { u, v };
}

Ray Camera::CreateRay(const Size pixel, const float randomMultiplier) const
{
	const auto uv = PixelToUV(pixel);
	const auto position = GetPixelPosition(uv[0], uv[1]);

	const float r1 = Random() - 0.5f;
	const float r2 = Random() - 0.5f;
	const float r3 = Random() - 0.5f;

	const float rx = (position - XForm.GetPosition())[0] + (r1 * randomMultiplier);
	const float ry = (position - XForm.GetPosition())[1] + (r2 * randomMultiplier);
	const float rz = (position - XForm.GetPosition())[2] + (r3 * randomMultiplier);

	return Ray(XForm.GetPosition(), { rx, ry, rz });
}

void Camera::Initialise()
{
	mViewport[0] = Matrix<float>(0.0, static_cast<Size>(Height), static_cast<Size>(Width));
	mViewport[1] = Matrix<float>(0.0, static_cast<Size>(Height), static_cast<Size>(Width));
	mViewport[2] = Matrix<float>(0.0, static_cast<Size>(Height), static_cast<Size>(Width));
}
