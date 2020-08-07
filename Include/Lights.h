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

			enum class Models
			{
				PHONG
			};

			virtual Ray Direction(const Vector3& hit) const = 0;
			virtual float Shadow(const std::vector<std::shared_ptr<Object>>& objects, const Vector3& hit) const = 0;

			Models Model = Models::PHONG;
			Vector3 Colour = { 1.0, 1.0, 1.0 };
			float ShadowIntensity = 0.4f;
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
			Area() : 
				Grid(std::make_shared<Plane>()) 
			{ 
				Grid->Material.Colour = Colour; 
			}
			Area(const float width, const float height, const Size samples = 8u) : 
				Area()
			{
				Grid->Width = width;
				Grid->Height = height;
				Samples = samples;
			}
			~Area() = default;

			// Samples squared.
			Size Samples = 8u;
			std::shared_ptr<Plane> Grid = nullptr;
			bool RenderGeometry = false;

			virtual Ray Direction(const Vector3& hit) const override;
			virtual float Shadow(const std::vector<std::shared_ptr<Object>>& objects, const Vector3& hit) const override;

			Vector3 SamplePlane(const float u, const float v, const Size uRegion, const Size vRegion, const float surfaceOffset = 0.0f) const;
		};
	}
}