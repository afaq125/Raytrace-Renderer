#include "Renderer.h"

using namespace Renderer;
using namespace Renderer::Math;

namespace
{
	std::default_random_engine Generator;
	std::uniform_real_distribution<float> Distribution(0, 1);
}

Transform::Transform(const Vector3& direction, const Vector3& up, const Vector3& position) :
	mAxis(Matrix3()),
	mPosition(position)
{
	float difference = direction.DotProduct(up);
	if (difference == 1.0f || difference == -1.0f)
	{
		std::cout << "Warning direction and up are perpendicular when creating axis. Returning identity matrix." << std::endl;
		mAxis.Identity();
		return;
	}

	auto z = direction.Normalized();
	auto y = z.CrossProduct(up).Normalized();
	auto x = z.CrossProduct(y).Normalized();
	for (Size i = 0; i < 3; ++i)
	{
		mAxis.Set(i, 0, y[i]);
		mAxis.Set(i, 1, z[i]);
		mAxis.Set(i, 2, x[i]);
	}
}

Vector3 Ray::Projection(const Vector3& position) const
{
	const float distance = (position - mOrigin).DotProduct(mDirection);
	return mOrigin + (mDirection * distance);
}

Vector3 Ray::Reflection(const Vector3& normal, const Vector3& direction)
{
	return (((normal - direction) * normal.DotProduct(direction)) * 2.0);
}

std::vector<Intersection> Renderer::IntersectScene(const std::vector<std::shared_ptr<Object>>& objects, const Ray& ray, bool checkAll)
{
	std::vector<Intersection> intersections;
	float distance = Infinity;
	for (const auto& o : objects)
	{
		Intersection intersect = o->Intersect(ray);
		if (intersect.Hit)
		{
			if (!checkAll)
			{
				return { intersect };
			}

			intersections.push_back(intersect);
			const float hitDistance = ray.GetOrigin().Distance(intersect.Position);
			if (hitDistance < distance)
			{
				distance = hitDistance;
				// Only care if front element is the closest;
				std::swap(intersections.front(), intersections.back());
			}
		}
	}
	return intersections;
}

float Renderer::Random()
{
	return Distribution(Generator);
}

Vector3 Renderer::SampleHemisphere(const float r1, const float r2)
{
	float angle = std::sqrt(1.0f - (r1 * r1));
	float phi = 2.0f * PI * r2;
	float x = angle * std::cos(phi);
	float z = angle * std::sin(phi);
	return { x, r1, z };
}

Vector3 Renderer::SampleCircle(const float r)
{
	float phi = 2.0f * PI * r;
	float x = std::cos(phi);
	float z = std::sin(phi);
	return { x, 0.0f, z };
}
