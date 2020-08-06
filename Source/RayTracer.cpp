#include "Renderer.h"

using namespace Renderer;
using namespace Renderer::Math;
using namespace Renderer::Lights;

namespace
{
	std::chrono::seconds CurrentTime()
	{
		auto now = std::chrono::system_clock::now();
		auto epoch = std::chrono::time_point_cast<std::chrono::seconds>(now).time_since_epoch();
		return std::chrono::duration_cast<std::chrono::seconds>(epoch);
	}
}

void RayTracer::Initialise()
{
	mBackgroundColour = { 0.0f, 0.0f, 0.0f };
	mSamplesPerPixel = 2u;
	mMaxDepth = 20u;
	mMaxGIDepth = 1u;
	mSecondryBounces = 10u;
}

Camera::Viewport RayTracer::Render(
	const std::function<void(const Camera::Viewport&, const std::string&)>& save,
	const std::string& path)
{
	auto job = [&](const Size i) -> void
	{
		auto colour = Vector3();
		for (Size s = 0; s < mSamplesPerPixel; ++s)
		{
			const auto ray = mCamera.CreateRay(i);
			const auto raytrace = Trace(ray);
			colour += raytrace.SurfaceColour;
		}
		colour *= 1.0f / static_cast<float>(mSamplesPerPixel);
		colour.Clamp(0.0f, 0.9999f);

		mCamera.SetPixel(i, colour[0], colour[1], colour[2]);
	};

	const auto start = CurrentTime();

	// Render
	const Size buckets = static_cast<Size>(mCamera.GetViewportArea());
	ThreadPool::RunWithCallback(job, [&]() { save(mCamera.GetViewport(), path); std::this_thread::sleep_for(std::chrono::seconds(10)); }, buckets);

	const auto end = CurrentTime();
	std::cout << "Start: " << start.count() << std::endl;
	std::cout << "End: " << end.count() << std::endl;
	std::cout << "Taken: " << (end.count() - start.count()) / 60.0 << std::endl;

	return mCamera.GetViewport();
}

Intersection RayTracer::Trace(const Ray& ray, const Size depth) const
{
	auto intersections = IntersectScene(mObjects, ray, true);

	if (intersections.empty())
	{
		return { false, Vector3(), mBackgroundColour, nullptr };
	}

	auto illumination = Vector3();
	auto intersection = intersections.front();
	const auto object = intersection.Object;
	const auto normal = object->CalculateNormal(intersection.Position);
	const auto hit = intersection.Position + (normal * 0.0001f);

	for (const auto& light : mLights)
	{
		float shadow = light->Shadow(mObjects, hit);

		if (object->Material.Reflective && depth < mMaxDepth)
		{
			auto trace = [&](const Ray& ray, const Size depth) -> Intersection { return RayTracer::Trace(ray, depth); };
			illumination += object->Material.BRDF(ray, normal, hit, trace, depth);
		}
		else
		{
			const auto direct = object->Material.BSDF(ray, normal, hit, shadow, &(*light));
			Vector3 indirect = 0.0f;

			if (depth < mMaxGIDepth)
			{
				const float pdf = 1.0f / (2.0f * PI);
				const auto axis = Transform(normal, (ray.GetOrigin() - hit).Normalized(), hit);
				for (Size i = 0; i < mSecondryBounces; ++i)
				{
					const float random1 = Random();
					const float random2 = Random();

					const Vector3 hemisphereSample = SampleHemisphere(random1, random2);
					const Vector3 hemisphereSampleToWorldSpace = hemisphereSample.MatrixMultiply(axis.GetAxis());
					const Ray indirectRay(axis.GetPosition(), hemisphereSampleToWorldSpace);

					auto giIntersection = Trace(indirectRay, depth + 1);
					auto colour = (giIntersection.SurfaceColour);// *r1) / pdf;
					colour.SetNaNsOrINFs(0.0);
					indirect += colour;
				}
				indirect /= static_cast<float>(mSecondryBounces);
				indirect /= static_cast<float>(depth + 1);
			}

			illumination += (direct / PI) + (indirect * 1.0f);
		}
	}

	illumination.Clamp(0.0, 1.0);
	intersection.SurfaceColour = illumination;
	return intersection;
}
