#include "Renderer.h"

using namespace Renderer;
using namespace Renderer::Math;
using namespace Renderer::Lights;

Vector3 Light::Attenuation(const Vector3& colour, const float intensity, const float distance) const
{
	const auto attenuation = 1.0f / (distance * distance);
	return (colour * intensity) * attenuation;
}

float Point::Shadow(const std::vector<std::shared_ptr<Object>>& objects, const Vector3& hit) const
{
	const auto ray = Ray(hit, XForm.GetPosition() - hit);
	const bool shadow = !IntersectScene(objects, ray, false).empty();
	return shadow ? 1.0f - ShadowIntensity : 1.0f;
}

Sample Point::Sampler(const Vector3& origin, const Vector3& direction, const Vector3& up, const float roughness) const
{
	const auto rayDirection = XForm.GetPosition() - origin;
	Ray sample(origin, rayDirection);
	return { sample, Colour * Intensity, rayDirection.Length() };
}

float Area::Shadow(const std::vector<std::shared_ptr<Object>>& objects, const Vector3& hit) const
{
	float shadow = 0.0f;
	Size samples = static_cast<Size>(std::round(std::sqrt(static_cast<float>(Samples))));
	for (Size u = 0; u < samples; ++u)
	{
		for (Size v = 0; v < samples; ++v)
		{
			float offset = 0.0f;
			if (RenderGeometry)
			{
				offset = 1.5f;
			}
			const float random1 = Random();
			const float random2 = Random();
			const auto position = SamplePlane(random1, random2, u, v, offset);
			const auto direction = (position - hit).Normalized();
			if (!IntersectScene(objects, Ray(hit, direction), false).empty())
			{
				shadow += 1.0f;
			}
		}
	}
	shadow = 1.0f - (shadow / std::pow(static_cast<float>(samples), 2.0f));
	shadow *= 1.0f - ShadowIntensity;
	return shadow;
}

Sample Area::Sampler(const Vector3& origin, const Vector3& direction, const Vector3& up, const float roughness) const
{
	Ray sample(origin, direction);
	return { sample, Colour * Intensity, Grid->XForm.GetPosition().Distance(origin) };
}

Vector3 Area::SamplePlane(const float u, const float v, const Size uRegion, const Size vRegion, const float surfaceOffset) const
{
	const float step = 1.0f / static_cast<float>(Samples);
	const float uOffset = step * uRegion;
	const float vOffset = step * vRegion;
	return Grid->UVToWorld(step + uOffset, step + vOffset, surfaceOffset);
}

float Enviroment::Shadow(const std::vector<std::shared_ptr<Object>>& objects, const Vector3& hit) const
{
	return 1.0f;
}

Sample Enviroment::Sampler(const Vector3& origin, const Vector3& direction, const Vector3& up, const float roughness) const
{
	const auto axis = Transform(direction, up, { 0.0f,0.0f,0.0f });
	
	const float random1 = Random();
	const float random2 = Random();

	Vector3 hemisphereSample = 0.0f;
	if (SamplerType == Sampler::SAMPLE_HEMISPHERE)
	{
		hemisphereSample = SampleHemisphere(random1, random2);
	}
	else
	{
		hemisphereSample = ImportanceSampleHemisphereGGX(random1, random2, roughness);
	}

	const Vector3 hemisphereSampleToWorldSpace = hemisphereSample.MatrixMultiply(axis.GetAxis());
	Ray sampleRay({ 0.0f,0.0f,0.0f }, hemisphereSampleToWorldSpace);
	const auto intersection = SampleCubeMap(sampleRay);
	const auto colour = intersection.SurfaceColour * Intensity;
	const auto distance = 1.0f;
	return { sampleRay, colour, distance };
}

Intersection Enviroment::SampleCubeMap(const Ray& ray) const
{
	for (const auto& plane : CubeMap)
	{
		auto intersection = plane.Intersect(ray);
		if(intersection.Hit)
		{
			const auto uv = plane.WorldToUV(intersection.Position);
			intersection.SurfaceColour = plane.Material.DiffuseTexture.Sample(uv[0], uv[1]);
			return intersection;
		}
	}
	return Intersection();
}

void Enviroment::SetCubeMapPixel(const Ray& ray, const Vector3& rgb)
{
	for (auto& plane : CubeMap)
	{
		auto intersection = plane.Intersect(ray);
		if (intersection.Hit)
		{
			const auto uv = plane.WorldToUV(intersection.Position);
			plane.Material.DiffuseTexture.SetPixel(uv[0], uv[1], rgb);
		}
	}
}

std::vector<Plane> Enviroment::GenerateCubeMap(
	Texture top, 
	Texture bottom, 
	Texture left, 
	Texture right, 
	Texture back, 
	Texture front)
{
	std::vector<Plane> cube;
	const auto WH = 1.1f;
	const auto halfWH = WH / 2.0f;

	cube.emplace_back(Plane(WH, WH, { 0.0f, halfWH, 0.0f }, { 0.0f, -1.0f, 0.0f }));
	cube.emplace_back(Plane(WH, WH, { 0.0f, -halfWH, 0.0f }, { 0.0f, 1.0f, 0.0f }));
	cube.emplace_back(Plane(WH, WH, { halfWH, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f }));
	cube.emplace_back(Plane(WH, WH, { -halfWH, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }));
	cube.emplace_back(Plane(WH, WH, { 0.0f, 0.0f, -halfWH }, { 0.0f, 0.0f, 1.0f }));
	cube.emplace_back(Plane(WH, WH, { 0.0f, 0.0f, halfWH }, { 0.0f, 0.0f, -1.0f }));

	cube[0].Material.DiffuseTexture = std::move(top);
	cube[1].Material.DiffuseTexture = std::move(bottom);
	cube[2].Material.DiffuseTexture = std::move(left);
	cube[3].Material.DiffuseTexture = std::move(right);
	cube[4].Material.DiffuseTexture = std::move(back);
	cube[5].Material.DiffuseTexture = std::move(front);

	cube[0].Material.Albedo = {1.0f, 0.0f, 0.0f};
	cube[1].Material.Albedo = {0.0f, 1.0f, 0.0f};
	cube[2].Material.Albedo = {0.0f, 0.0f, 1.0f};
	cube[3].Material.Albedo = {0.0f, 0.0f, 1.0f};
	cube[4].Material.Albedo = {0.0f, 1.0f, 0.0f};
	cube[5].Material.Albedo = {1.0f, 0.0f, 0.0f};

	return cube;
}