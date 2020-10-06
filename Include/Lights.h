#pragma once

namespace Renderer
{
	namespace Lights
	{
		using namespace Math;

		struct Sample
		{
			Ray IncomingRay;
			Vector3 Colour;
			float Distance;
		};

		struct SamplerSettings
		{
			enum class Sampler
			{
				SAMPLE_HEMISPHERE,
				SAMPLE_HEMISPHERE_GGX
			};

			Sampler SamplerType = Sampler::SAMPLE_HEMISPHERE;
			float Roughness;
		};

		class Light
		{
		public:
			Light() = default;
			virtual ~Light() = default;

			virtual float Shadow(const std::vector<std::shared_ptr<Object>>& objects, const Vector3& hit) const = 0;
			// TODO: Change this so its easier to select the sampler type in the shader object. Maybe have a sampler object that can be passed in.
			virtual Sample Sampler(const Vector3& origin, const Vector3& direction, const Vector3& up, const SamplerSettings& settings) const = 0;
			virtual Vector3 Attenuation(const Vector3& colour, const float intensity, const float distance) const;

			float Intensity = 1.0f;
			Vector3 Colour = { 1.0, 1.0, 1.0 };
			float ShadowIntensity = 0.4f;
			Size Samples = 8u;
		};

		class Point : public Light
		{
		public:
			Point() 
			{
				Samples = 1;
			}

			virtual float Shadow(const std::vector<std::shared_ptr<Object>>& objects, const Vector3& hit) const override;
			virtual Sample Sampler(const Vector3& origin, const Vector3& direction, const Vector3& up, const SamplerSettings& settings) const override;

			Transform XForm;
		};

		class Area : public Light
		{
		public:
			Area() : 
				Grid(std::make_shared<Plane>()) 
			{ 
				Grid->Material.Albedo = Colour;
				Samples = 64;
			}
			Area(const float width, const float height, const Size samples = 64u) : 
				Area()
			{
				Grid->Width = width;
				Grid->Height = height;
				Samples = samples;
			}
			virtual ~Area() = default;

			std::shared_ptr<Plane> Grid = nullptr;
			bool RenderGeometry = false;

			virtual float Shadow(const std::vector<std::shared_ptr<Object>>& objects, const Vector3& hit) const override;
			virtual Sample Sampler(const Vector3& origin, const Vector3& direction, const Vector3& up, const SamplerSettings& settings) const override;

			Vector3 SamplePlane(const float u, const float v, const Size uRegion, const Size vRegion, const float surfaceOffset = 0.0f) const;
		};

		class Enviroment : public Light
		{
		public:
			Enviroment()
			{
				Samples = 32;
			}
			Enviroment(Texture top,
				Texture bottom,
				Texture left,
				Texture right,
				Texture back,
				Texture front) :
				Enviroment()
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

			virtual float Shadow(const std::vector<std::shared_ptr<Object>>& objects, const Vector3& hit) const override;
			virtual Sample Sampler(const Vector3& hit, const Vector3& view, const Vector3& normal, const SamplerSettings& settings) const override;

			Intersection SampleCubeMap(const Ray& ray) const;
			void SetCubeMapPixel(const Ray& ray, const Vector3& rgb);

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