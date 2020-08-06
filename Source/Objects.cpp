#include "Renderer.h"

using namespace Renderer;
using namespace Renderer::Math;
using namespace Renderer::Lights;

Vector3 Plane::UVToWorld(const float u, const float v) const
{
	const auto x = Width * u;
	const auto y = Height * v;
	const auto halfWidth = Width / 2.0f;
	const auto halfHeight = Height / 2.0f;
	const Vector3 local = { x - halfWidth, 0.0f, y - halfHeight };
	const auto world = local.MatrixMultiply(XForm.GetAxis());
	return world + XForm.GetPosition();
}

void Plane::SetDirection(const Vector3& direction)
{
	const auto normalised = direction.Normalized();
	float difference = normalised.DotProduct(Y_AXIS);
	if (difference == 1.0f)
	{
		return;
	}

	if (difference == -1.0f)
	{
		Matrix3 axis = XForm.GetAxis();
		axis *= -1.0f;
		XForm.SetAxis(axis);
		return;
	}

	XForm = Transform(normalised, Y_AXIS, XForm.GetPosition());
}

Vector3 Plane::CalculateNormal(const Vector3& hit) const
{
	const float x = XForm.GetAxis().Get(0, 1);
	const float y = XForm.GetAxis().Get(1, 1);
	const float z = XForm.GetAxis().Get(2, 1);
	return { x, y, z };
}

Intersection Plane::Intersect(const Ray& ray) const 
{
	const float x = XForm.GetAxis().Get(0, 1);
	const float y = XForm.GetAxis().Get(1, 1);
	const float z = XForm.GetAxis().Get(2, 1);
	const Vector3 normal = { x, y, z };

	const auto difference = ray.GetDirection().DotProduct(normal);

	if (std::abs(difference) > 0.0f)
	{
		const auto direction = XForm.GetPosition() - ray.GetOrigin();
		const auto surfaceDistance = direction.DotProduct(normal) / difference;
		const bool hit = surfaceDistance >= 0.0f;
		if (hit)
		{
			const Vector3 position = (ray.GetDirection() * surfaceDistance) + ray.GetOrigin();
			const auto pLocal = position - XForm.GetPosition();

			const auto halfWidth = Width / 2.0f;
			const auto halfHeight = Height / 2.0f;
			const Vector3 xLocal = { halfWidth, 0.0f, 0.0f };
			const Vector3 yLocal = { 0.0f, 0.0f, halfHeight };
			const auto xWorld = xLocal.MatrixMultiply(XForm.GetAxis());
			const auto yWorld = yLocal.MatrixMultiply(XForm.GetAxis());

			const auto xDistance = xWorld.DotProduct(pLocal) / halfWidth;
			const auto yDistance = yWorld.DotProduct(pLocal) / halfHeight;

			if (xDistance > -halfWidth && xDistance < halfWidth &&
				yDistance > -halfHeight && yDistance < halfHeight)
			{
				return { true,  position, Material.Colour, static_cast<const Object*>(this) };
			}
		}
	}

	return Intersection();
}

Intersection Sphere::Intersect(const Ray& ray) const
{
	const auto sphereToRay = XForm.GetPosition() - ray.GetOrigin();
	const auto sign = sphereToRay.DotProduct(ray.GetDirection());

	if (sign < 0.0f)
	{
		if (sphereToRay.Length() > Radius)
		{
			return Intersection();
		}
		if (sphereToRay.Length() == Radius)
		{
			// TODO: Check this but it should be the case when the ray origin is on the edge of the sphere.
			return { true, ray.GetOrigin(), Material.Colour, static_cast<const Object*>(this) };
		}
	}

	const Vector3 projection = ray.Projection(XForm.GetPosition());
	if ((XForm.GetPosition() - projection).Length() > Radius)
	{
		return Intersection();
	}

	const Vector3 projMinusPos = projection - XForm.GetPosition();
	const float distance = std::sqrt((Radius * Radius) - (projMinusPos.Length() * projMinusPos.Length()));

	float offset = (projection - ray.GetOrigin()).Length();
	if (sphereToRay.Length() > Radius)
	{
		offset -= distance;
	}
	else
	{
		offset += distance;
	}

	return { true, ray.GetOrigin() + (ray.GetDirection() * offset), Material.Colour, static_cast<const Object*>(this) };
}

Vector3 Sphere::CalculateNormal(const Vector3& hit) const
{
	return (hit - XForm.GetPosition()).Normalized();
}