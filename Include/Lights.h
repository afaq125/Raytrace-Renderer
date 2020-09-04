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
			virtual ~Light() = default;

			virtual Ray Direction(const Vector3& hit) const = 0;
			virtual float Shadow(const std::vector<std::shared_ptr<Object>>& objects, const Vector3& hit) const = 0;
			virtual Vector3 IntensityAt(const Ray& ray) const = 0;

			Vector3 Colour = { 1.0, 1.0, 1.0 };
			float ShadowIntensity = 0.4f;
		};

		class Point : public Light
		{
		public:
			virtual Ray Direction(const Vector3& hit) const override;
			virtual float Shadow(const std::vector<std::shared_ptr<Object>>& objects, const Vector3& hit) const override;
			virtual Vector3 IntensityAt(const Ray& ray) const override;

			Transform XForm;
		};

		class Area : public Light
		{
		public:
			Area() : 
				Grid(std::make_shared<Plane>()) 
			{ 
				Grid->Material.Diffuse = Colour; 
			}
			Area(const float width, const float height, const Size samples = 8u) : 
				Area()
			{
				Grid->Width = width;
				Grid->Height = height;
				Samples = samples;
			}
			virtual ~Area() = default;

			// Samples squared.
			Size Samples = 8u;
			std::shared_ptr<Plane> Grid = nullptr;
			bool RenderGeometry = false;

			virtual Ray Direction(const Vector3& hit) const override;
			virtual float Shadow(const std::vector<std::shared_ptr<Object>>& objects, const Vector3& hit) const override;
			virtual Vector3 IntensityAt(const Ray& ray) const override;

			Vector3 SamplePlane(const float u, const float v, const Size uRegion, const Size vRegion, const float surfaceOffset = 0.0f) const;
		};

		class Enviroment : public Light
		{
		public:
			Enviroment() = default;
			Enviroment(Texture top,
				Texture bottom,
				Texture left,
				Texture right,
				Texture back,
				Texture front)
			{
				CubeMap = GenerateCubeMap(
					std::move(top),
					std::move(bottom),
					std::move(left),
					std::move(right),
					std::move(back),
					std::move(front));
			}
			virtual ~Enviroment() = default;

			std::vector<Plane> CubeMap;

			virtual Ray Direction(const Vector3& hit) const override;
			virtual float Shadow(const std::vector<std::shared_ptr<Object>>& objects, const Vector3& hit) const override;
			virtual Vector3 IntensityAt(const Ray& ray) const override;

			Vector3 SampleCubeMap(const Ray& ray) const;

			static std::vector<Plane> GenerateCubeMap(
				Texture top,
				Texture bottom,
				Texture left,
				Texture right,
				Texture back,
				Texture front);
		};
	}
}