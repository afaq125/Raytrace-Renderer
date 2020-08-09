#include "Renderer.h"

using namespace Renderer;
using namespace Renderer::Math;
using namespace Renderer::Lights;

Vector3 Shader::BSDF(const Ray& ray, const Vector3& normal, const Vector3& hit, const float shadow, const Light* light, const std::function<Intersection(const Ray&)>& trace) const
{

	return BRDF(ray, normal, hit, trace);

	const auto lightDirection = light->Direction(hit).GetDirection();
	const float lambertian = std::max(normal.DotProduct(lightDirection), 0.2f);
	const auto diffuse = Diffuse * lambertian;
	const auto specular = Phong(ray.GetDirection(), hit, light, normal);
	const auto direct = (diffuse + specular + (light->Colour * 0.4f)) * shadow;
	return direct;
}

Vector3 Shader::BRDF(const Ray& ray, const Vector3& normal, const Vector3& hit, const std::function<Intersection(const Ray&)>& trace) const
{
	//const auto axis = Transform(normal, (ray.GetOrigin() - hit).Normalized(), hit);
	//const float random = Random();
	//const Vector3 circleSample = SampleCircle(random);
	//const Vector3 circleSampleToWorldSpace = circleSample.MatrixMultiply(axis.GetAxis());
	//const Vector3 newHit = hit + circleSampleToWorldSpace;

	//const auto reflectionRay = Ray(hit, Ray::Reflection(normal, (ray.GetOrigin() - newHit).Normalized()));

	//auto reflection = trace(reflectionRay).SurfaceColour * 1.0f;

	//float facingRatio = ray.GetDirection().DotProduct(hit);
	//float fresnel = mix(std::pow(1.0f - facingRatio, 3), 1.0f, 0.1f);

	//return (reflection)* Diffuse;

	float facingRatio = ray.GetDirection().DotProduct(normal);

	float F0 = abs((1.0 - IOR) / (1.0 + IOR));
	F0 = F0 * F0;
	F0 = mix(Diffuse[0], Metalness, F0);

	float fresnel = Fresnel(facingRatio, F0);

	return fresnel;
}

float Shader::Fresnel(const float incidenceAngle, float ior) const
{
	return ior + (1 - ior) * std::pow(1 - incidenceAngle, 5);
}

Vector3 Shader::Geometry() const
{

	return 0.0f;
}

Vector3 Shader::Distribution() const
{

	return 0.0f;
}

Vector3 Shader::Phong(const Vector3& view, const Vector3& hit, const Light* light, const Vector3& normal) const
{
	const auto reflection = Ray::Reflection(normal, light->Direction(hit).GetDirection()) * -1.0f;
	auto specularAngle = std::max(reflection.DotProduct(view), 0.0f);
	auto specular = std::pow(specularAngle, 2.0f);
	return light->Colour * specular;
}