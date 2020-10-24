#pragma once

namespace Renderer
{
	namespace Lights
	{
		class Light;
	}

	using namespace Math;
	using namespace Lights;

	class Texture
	{
	public:
		Texture() = default;
		Texture(const Size width, const Size height)
		{
			for (auto& channel : Pixels)
			{
				channel = Matrix<float>(height, width);
			}
		}
		~Texture() = default;

		using Data = std::array<Matrix<float>, 4>;
		Data Pixels;

		Vector3 Sample(const float u, const float v) const;
		void SetPixel(const float u, const float v, const Vector3& rgb);
	};

	class Shader
	{
	public:
		Shader() = default;
		virtual ~Shader() {};

		Vector3 Albedo = { 1.0, 0.0, 0.0 };
		float Roughness = 0.1f;
		float Metalness = 1.0f;
		float IOR = 1.2f;
		float Emission = 0.0f;
		Vector3 Displacement = { 0.0f, 0.0f, 0.0f };
		Size ReflectionDepth = 1u;
		Size ReflectionSamples = 16u;

		Texture DiffuseTexture;

		Vector3 BSDF(const Ray& ray, 
			const Vector3& normal, 
			const Vector3& hit, 
			const std::vector<std::shared_ptr<Object>>& objects, 
			const std::vector<std::shared_ptr<Light>>& lights) const;

		Vector3 BRDF(const Ray& ray, 
			const Vector3& normal, 
			const Vector3& hit, 
			const std::vector<std::shared_ptr<Object>>& objects, 
			const std::vector<std::shared_ptr<Light>>& lights) const;

		float Shadow(const Vector3& hit,
			const std::vector<std::shared_ptr<Object>>& objects,
			const std::vector<std::shared_ptr<Light>>& lights) const;

		Vector3 SceneReflections(
			Vector3 origin,
			Vector3 hit,
			Vector3 normal,
			float roughness,
			const Size depth,
			const Size samples,
			const std::vector<std::shared_ptr<Object>>& objects) const;

	private:
		Vector3 Fresnel(const float incidenceAngle, const Vector3& ior) const;
		float Geometry(const Vector3& normal, const Vector3& view, const Vector3& lightDirection, const float k) const;
		float Distribution(const Vector3 normal, const Vector3 half, const float roughness) const;
	};

}