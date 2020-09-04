#include "Renderer.h"

using namespace Renderer;
using namespace Renderer::Math;
using namespace Renderer::Lights;

Vector3 Texture::Sample(const float u, const float v) const
{
	const auto rows = Pixels[0].Rows();
	const auto columns = Pixels[0].Columns();
	const Size x = u * rows;
	const Size y = v * columns;
	return { Pixels[0].Get(x, y), Pixels[1].Get(x, y), Pixels[2].Get(x, y) };
}

Vector3 Shader::BSDF(const Ray& ray, const Vector3& normal, const Vector3& hit, const float shadow, const Light* light, const std::function<Intersection(const Ray&)>& trace) const
{
	const auto reflection = Ray::Reflection(normal, (ray.GetOrigin() - hit).Normalized());
	return light->IntensityAt(Ray({0.0f,0.0f,0.0f}, reflection));

	return BRDF(ray, normal, hit, shadow, light, trace);

	const auto lightDirection = light->Direction(hit).GetDirection();
	const float lambertian = std::max(lightDirection.DotProduct(normal), 0.2f);
	const auto diffuse = Diffuse * lambertian;
	const auto specular = Phong(ray.GetDirection(), hit, light, normal);
	const auto direct = (diffuse + specular + (light->Colour * 0.4f)) * shadow;
	return direct;
}

Vector3 Shader::BRDF(const Ray& ray, const Vector3& normal, const Vector3& hit, const float shadow, const Light* light, const std::function<Intersection(const Ray&)>& trace) const
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

	/////

	Vector3 t = 0.0f;
	const auto axis = Transform(normal, (ray.GetOrigin() - hit).Normalized(), hit);
	for (Size i = 0; i < 1; ++i)
	{
		const float random1 = Random();
		const float random2 = Random();

		const Vector3 hemisphereSample = SampleHemisphere(random1, random2);
		const Vector3 hemisphereSampleToWorldSpace = hemisphereSample.MatrixMultiply(axis.GetAxis());
		const Ray indirectRay(axis.GetPosition(), hemisphereSampleToWorldSpace);

		//auto giIntersection = Trace(indirectRay, depth + 1);
		//auto colour = (giIntersection.SurfaceColour);// *r1) / pdf;
		//colour.SetNaNsOrINFs(0.0);
		//indirect += colour;

		///// 

		const auto lightDirection = light->Direction(hit).GetDirection();
		const float lambertian = std::max(lightDirection.DotProduct(normal), 0.2f);
		const auto diffuse = Diffuse * lambertian;
		const auto specular = Phong(ray.GetDirection(), hit, light, normal);
		const auto direct = (diffuse + specular + (light->Colour * 0.4f)) * shadow;
		const auto D = direct;

		/////

		float facingRatio = ray.GetDirection().DotProduct(normal);

		float F0 = abs((1.0 - IOR) / (1.0 + IOR));
		F0 = F0 * F0;
		F0 = mix(Diffuse[0], Metalness, F0);

		float F = Fresnel(facingRatio, F0);

		/////

		//const auto lightDirection = light->Direction(hit).GetDirection();
		const auto G = Geometry(normal, ray.GetDirection(), lightDirection, 0.5f);

		/////

		const auto DFG = D * F * G;

		/////

		t += DFG;
	}

	////

	auto a = ray.GetDirection() * -1.0f;
	float NDotV = std::max(normal.DotProduct(a), 0.0f);
	float denominator = Clamp(4.0f * ((NDotV *NDotV) + 0.05f), 0.0f, 1.0f);

	return t / denominator;
}

float Shader::Fresnel(const float incidenceAngle, float ior) const
{
	return ior + (1 - ior) * std::pow(1 - incidenceAngle, 5);
}

Vector3 Shader::Geometry(const Vector3& normal, const Vector3& view, const Vector3& lightDirection, const float k) const
{
	auto lSchlickGGXD = [&](const float NDotV, const float k) -> float
	{
		return NDotV / (NDotV * (1.0f - k) + k);
	};

	// GeometrySmith
	auto a = view * -1.0f;
	float NDotV = std::max(normal.DotProduct(a), 0.0f);
	float NDotL = std::max(normal.DotProduct(lightDirection), 0.0f);
	float GGX1 = lSchlickGGXD(NDotV, k);
	float GGX2 = lSchlickGGXD(NDotL, k);
	return GGX1 * GGX2;
}

Vector3 Shader::Distribution() const
{

	return 0.0f;
}

Vector3 Shader::Phong(const Vector3& view, const Vector3& hit, const Light* light, const Vector3& normal) const
{
	const auto reflection = Ray::Reflection(normal, light->Direction(hit).GetDirection()) * -1.0f;
	auto specularAngle = std::max(view.DotProduct(reflection), 0.0f);
	auto specular = std::pow(specularAngle, 32.0f);
	return light->IntensityAt(Ray(hit, reflection)) * specular;
}