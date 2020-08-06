#pragma once

namespace Renderer
{
	namespace Lights
	{
		class Light;
	}

	using namespace Math;
	using namespace Lights;

	class Shader
	{
	public:
		Shader() = default;
		~Shader() = default;

		Vector3 Colour = { 1.0, 0.0, 0.0 };
		bool Reflective = false;

		Vector3 BSDF(const Ray& ray, const Vector3& normal, const Vector3& hit, const float shadow, const Light* light) const;
		Vector3 BRDF(const Ray& ray, const Vector3& normal, const Vector3& hit, const std::function<Intersection(const Ray&, const Size)>& trace, const Size depth) const;
	};

	Vector3 Phong(const Vector3& view, const Vector3& hit, const Light* light, const Vector3& normal);
}