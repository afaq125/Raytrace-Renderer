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

void Scene::Initialise()
{
	for (const auto& light : Lights)
	{
		std::shared_ptr<Lights::Area> area = std::dynamic_pointer_cast<Lights::Area>(light);
		if (area != nullptr && area->RenderGeometry)
		{
			Objects.push_back(area->Grid);
		}
	}
}

Camera::Viewport RayTracer::Render(
	const std::function<void(const Camera::Viewport&, const std::string&)>& save,
	const std::string& path)
{
	auto job = [&](const Size i) -> void
	{
		auto colour = Vector3();
		for (Size s = 0; s < mSettings.SamplesPerPixel; ++s)
		{
			const auto ray = mCamera.CreateRay(i);
			const auto raytrace = Trace(ray);
			colour += raytrace.SurfaceColour;
		}
		colour *= 1.0f / static_cast<float>(mSettings.SamplesPerPixel);
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
	if (depth > mSettings.MaxDepth)
	{
		return Intersection();
	}

	const auto intersections = IntersectScene(mScene.get().Objects, ray, true);

	if (intersections.empty())
	{
		return { false, Vector3(), mSettings.BackgroundColour, nullptr };
	}

	auto intersection = intersections.front();
	const auto object = intersection.Object;
	const auto normal = object->CalculateNormal(intersection.Position);
	const auto hit = intersection.Position + (normal * 0.0001f);

	Vector3 direct = 0.0f;
	Vector3 indirect = 0.0f;

	direct += object->Material.BSDF(ray, normal, hit, mScene.get().Objects, mScene.get().Lights);

	if (depth < mSettings.MaxGIDepth)
	{                    
		indirect = GlobalIllumination(ray, normal, hit, depth);                                                   
	}

	const auto irradiance = ((direct / PI) + (indirect * 2.0f)) * object->Material.Albedo;

	intersection.SurfaceColour = irradiance;
	return intersection;
}

Vector3 RayTracer::GlobalIllumination(const Ray& ray, const Vector3& normal, const Vector3& hit, const Size depth) const 
{
	Vector3 indirect = 0.0f;
	constexpr float pdf = 1.0f / (2.0f * PI);
	const auto axis = Transform(normal, (ray.GetOrigin() - hit).Normalized(), hit);
	for (Size i = 0; i < mSettings.SecondryBounces; ++i)
	{
		const float random1 = Random();
		const float random2 = Random();

		const Vector3 hemisphereSample = SampleHemisphere(random1, random2);
		const Vector3 hemisphereSampleToWorldSpace = hemisphereSample.MatrixMultiply(axis.GetAxis());
		const Ray indirectRay(axis.GetPosition(), hemisphereSampleToWorldSpace);

		const auto giIntersection = Trace(indirectRay, depth + 1);
		const auto colour = (giIntersection.SurfaceColour * random1) / pdf;

		indirect += colour;
	}
	indirect *= (1.0f / static_cast<float>(mSettings.SecondryBounces));
	indirect *= (1.0f / static_cast<float>(depth + 1));
	return indirect;
}