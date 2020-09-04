#include "Renderer.h"

using namespace Renderer;
using namespace Renderer::Math;
using namespace Renderer::Lights;

Ray Point::Direction(const Vector3& hit) const
{
	return Ray(hit, XForm.GetPosition() - hit);
}

float Point::Shadow(const std::vector<std::shared_ptr<Object>>& objects, const Vector3& hit) const
{
	const auto ray = Direction(hit);
	const bool shadow = !IntersectScene(objects, ray, false).empty();
	return shadow ? 1.0f - ShadowIntensity : 1.0f;
}

Vector3 Point::IntensityAt(const Ray& ray) const
{
	return Colour;
}

Ray Area::Direction(const Vector3& hit) const
{
	return Ray(hit, Grid->XForm.GetPosition() - hit);
}

float Area::Shadow(const std::vector<std::shared_ptr<Object>>& objects, const Vector3& hit) const
{
	float shadow = 0.0f;
	for (Size u = 0; u < Samples; ++u)
	{
		for (Size v = 0; v < Samples; ++v)
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
	shadow = 1.0f - (shadow / std::pow(static_cast<float>(Samples), 2.0f));
	shadow *= 1.0f - ShadowIntensity;
	return shadow;
}

Vector3 Area::IntensityAt(const Ray& ray) const
{
	return Colour;
}


Vector3 Area::SamplePlane(const float u, const float v, const Size uRegion, const Size vRegion, const float surfaceOffset) const
{
	const float step = 1.0f / static_cast<float>(Samples);
	const float uOffset = step * uRegion;
	const float vOffset = step * vRegion;
	return Grid->UVToWorld(step + uOffset, step + vOffset, surfaceOffset);
}


Ray Enviroment::Direction(const Vector3& hit) const
{
	return Ray(NaN, NaN);
}

float Enviroment::Shadow(const std::vector<std::shared_ptr<Object>>& objects, const Vector3& hit) const
{
	return 1.0f;
}

Vector3 Enviroment::IntensityAt(const Ray& ray) const
{
	return SampleCubeMap(ray);
}

Vector3 Enviroment::SampleCubeMap(const Ray& ray) const
{
	for (const auto& plane : CubeMap)
	{
		const auto intersection = plane.Intersect(ray);
		if(intersection.Hit)
		{
			const auto uv = plane.WorldToUV(intersection.Position);
			return plane.Material.DiffuseTexture.Sample(uv[0], uv[1]);
		}
	}
	return { NaN, NaN, NaN };
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

	cube[0].Material.Diffuse = {1.0f, 0.0f, 0.0f};
	cube[1].Material.Diffuse = {0.0f, 1.0f, 0.0f};
	cube[2].Material.Diffuse = {0.0f, 0.0f, 1.0f};
	cube[3].Material.Diffuse = {0.0f, 0.0f, 1.0f};
	cube[4].Material.Diffuse = {0.0f, 1.0f, 0.0f};
	cube[5].Material.Diffuse = {1.0f, 0.0f, 0.0f};

	return cube;
}