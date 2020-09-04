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
	};

	class Shader
	{
	public:
		Shader() = default;
		~Shader() = default;

		Vector3 Diffuse = { 1.0, 0.0, 0.0 };
		float Roughness = 1.0f;
		float Metalness = 0.0f;
		float Specular = 1.0f;
		float IOR = 1.2f;
		float Emission = 0.0f;
		Vector3 Displacement = { 0.0f, 0.0f, 0.0f };

		Texture DiffuseTexture;

		Vector3 BSDF(const Ray& ray, const Vector3& normal, const Vector3& hit, const float shadow, const Light* light, const std::function<Intersection(const Ray&)>& trace) const;
		Vector3 BRDF(const Ray& ray, const Vector3& normal, const Vector3& hit, const float shadow, const Light* light, const std::function<Intersection(const Ray&)>& trace) const;

	private:
		float Fresnel(const float incidenceAngle, float ior) const;
		Vector3 Geometry(const Vector3& normal, const Vector3& view, const Vector3& lightDirection, const float k) const;
		Vector3 Distribution() const;

		Vector3 Phong(const Vector3& view, const Vector3& hit, const Light* light, const Vector3& normal) const;
	};

}