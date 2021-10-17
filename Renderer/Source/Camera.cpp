#include "Renderer.h"

using namespace Renderer;
using namespace Renderer::Math;

void Camera::LookAt(const Vector3& target, const Vector3& up)
{
	const auto direction = XForm.GetPosition() - target;
	const auto axis = Transform(direction, up, XForm.GetPosition(), true);
	XForm.SetAxis(axis.GetAxis());
}

Ray Camera::CreateRay(const Size pixel, const float randomMultiplier) const
{
	const auto uv = m_viewport.GetPixelUV(pixel);
	const auto xy = m_viewport.GetPixelPosition(uv[0], uv[1]);

    Vector3 position = { xy[0], -FocalLength, xy[1] };
    auto transformed = position.MatrixMultiply(XForm.GetAxis());
    transformed += XForm.GetPosition();

	const float r1 = Random() - 0.5f;
	const float r2 = Random() - 0.5f;
	const float r3 = Random() - 0.5f;

	const float rx = (transformed - XForm.GetPosition())[0] + (r1 * randomMultiplier);
	const float ry = (transformed - XForm.GetPosition())[1] + (r2 * randomMultiplier);
	const float rz = (transformed - XForm.GetPosition())[2] + (r3 * randomMultiplier);

	return Ray(XForm.GetPosition(), { rx, ry, rz });
}
