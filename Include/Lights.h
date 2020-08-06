#pragma once

namespace Renderer
{
	namespace Lights
	{
		using namespace Math;

		class Light
		{
		public:
			Light() = default;
			~Light() = default;

			virtual Ray Direction(const Vector3& hit) const = 0;
			virtual float Shadow(const std::vector<std::shared_ptr<Object>>& objects, const Vector3& hit) const = 0;

			Vector3 Colour = { 1.0, 1.0, 1.0 };
			float ShadowIntensity = 0.2f;
		};

		class Point : public Light
		{
		public:
			virtual Ray Direction(const Vector3& hit) const override;
			virtual float Shadow(const std::vector<std::shared_ptr<Object>>& objects, const Vector3& hit) const override;

			Transform XForm;
		};

		class Area : public Light
		{
		public:
			Area() = default;
			Area(const float width, const float height, const Size samples = 8u) : 
				Grid(Plane(width, height)),
				Samples(samples)
			{
				Grid.Material.Colour = Colour;
			}
			~Area() = default;

			// Samples squared.
			Size Samples = 8u;
			Plane Grid;

			virtual Ray Direction(const Vector3& hit) const override;
			virtual float Shadow(const std::vector<std::shared_ptr<Object>>& objects, const Vector3& hit) const override;

			Vector3 SamplePlane(const float u, const float v, const Size uRegion, const Size vRegion) const;
		};
	}
}