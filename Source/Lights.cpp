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

Vector3 Area::SamplePlane(const float u, const float v, const Size uRegion, const Size vRegion, const float surfaceOffset) const
{
	const float step = 1.0f / static_cast<float>(Samples);
	const float uOffset = step * uRegion;
	const float vOffset = step * vRegion;
	return Grid->UVToWorld(step + uOffset, step + vOffset, surfaceOffset);
}