#include "Renderer.h"

using namespace Renderer;
using namespace Renderer::Math;
using namespace Renderer::Lights;

Vector3 Plane::WorldToUV(const Vector3 position) const
{
	const auto& inverse = XForm.GetInverse();
	const Vector3 whLocal = { Width / 2.0f, 0.0f, Height / 2.0f };
	const auto local = (position - XForm.GetPosition()).MatrixMultiply(inverse) - whLocal;
	const auto u = std::abs(local[0] / Width);
	const auto v = std::abs(local[2] / Height);
	return { u, v, 0.0f };
}

Vector3 Plane::UVToWorld(const float u, const float v, const float surfaceOffset) const
{
	const auto x = Width * u;
	const auto y = Height * v;
	const auto halfWidth = Width / 2.0f;
	const auto halfHeight = Height / 2.0f;
	const Vector3 local = { x - halfWidth, surfaceOffset, y - halfHeight };
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

	XForm = Transform(normalised, Y_AXIS, XForm.GetPosition(), true);
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

            const auto& inverse = XForm.GetInverse();
            const auto local = pLocal.MatrixMultiply(inverse);

			const auto halfWidth = Width / 2.0f;
			const auto halfHeight = Height / 2.0f;
			//const Vector3 xLocal = { halfWidth, 0.0f, 0.0f };
			//const Vector3 yLocal = { 0.0f, 0.0f, halfHeight };
			//const auto xWorld = xLocal.MatrixMultiply(XForm.GetAxis());
			//const auto yWorld = yLocal.MatrixMultiply(XForm.GetAxis());

			//const auto xDistance = xWorld.DotProduct(pLocal) / halfWidth;
			//const auto yDistance = yWorld.DotProduct(pLocal) / halfHeight;

            const auto xDistance = local[0];
            const auto yDistance = local[2];

			if (xDistance > -halfWidth && xDistance < halfWidth &&
				yDistance > -halfHeight && yDistance < halfHeight)
			{
				return { true,  position, Material.Albedo, static_cast<const Object*>(this) };
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
			return { true, ray.GetOrigin(), Material.Albedo, static_cast<const Object*>(this) };
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

	return { true, ray.GetOrigin() + (ray.GetDirection() * offset), Material.Albedo, static_cast<const Object*>(this) };
}

Vector3 Sphere::CalculateNormal(const Vector3& hit) const
{
	return (hit - XForm.GetPosition()).Normalized();
}

Intersection Cube::Intersect(const Ray& ray) const
{
	const auto halfVector = { Width / 2.0f, Height / 2.0f, Length / 2.0f };
	const auto min = XForm.GetPosition() - halfVector;
	const auto max = XForm.GetPosition() + halfVector;

	const auto& origin = ray.GetOrigin();
	const auto& direction = ray.GetDirection();
	float t1 = (min[0] - origin[0]) * (1.0f / direction[0]);
	float t2 = (max[0] - origin[0]) * (1.0f / direction[0]);
	float t3 = (min[1] - origin[1]) * (1.0f / direction[1]);
	float t4 = (max[1] - origin[1]) * (1.0f / direction[1]);
	float t5 = (min[2] - origin[2]) * (1.0f / direction[2]);
	float t6 = (max[2] - origin[2]) * (1.0f / direction[2]);

	float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
	float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

	float distance = tmin;

	if (tmax < 0.0f || tmin < 0.0f)
	{
		return Intersection();
	}

	if (tmin > tmax)
	{
		return Intersection();
	}

	const Vector3 t = origin + (direction * distance);
	return { true, t, Material.Albedo, static_cast<const Object*>(this) };
}

Vector3 Cube::CalculateNormal(const Vector3& hit) const
{
	const auto local = hit - XForm.GetPosition();
	Vector3 normal = { 0.0f, 0.0f, 0.0f };
	
	if (IsNearValue(local[0], Width / 2.0f))
	{
		normal = { 1.0f, 0.0f, 0.0f };
		if (local[0] < 0.0f)
		{
			normal = { -1.0f, 0.0f, 0.0f };
		}
		return normal;
	}

	if (IsNearValue(local[1], Height / 2.0f))
	{
		normal = { 0.0f, 1.0f, 0.0f };
		if (local[1] < 0.0f)
		{
			normal = { 0.0f, -1.0f, 0.0f };
		}
		return normal;
	}

	if (IsNearValue(local[2], Length / 2.0f))
	{
		normal = { 0.0f, 0.0f, 1.0f };
		if (local[2] < 0.0f)
		{
			normal = { 0.0f, 0.0f, -1.0f };
		}
		return normal;
	}

	return local.Normalized();
}