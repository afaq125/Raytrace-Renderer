#include "Renderer.h"

using namespace Renderer;
using namespace Renderer::Math;
using namespace Renderer::Lights;

Vector3 Texture::Sample(const float u, const float v) const
{
	const auto rows = static_cast<float>(Pixels[0].Rows());
	const auto columns = static_cast<float>(Pixels[0].Columns());
	const Size x = static_cast<Size>(u * rows);
	const Size y = static_cast<Size>(v * columns);
	return { Pixels[0].Get(x, y), Pixels[1].Get(x, y), Pixels[2].Get(x, y) };
}

Vector3 Shader::BSDF(const Ray& ray, const Vector3& normal, const Vector3& hit, const float shadow, const Light* light) const
{
	return BRDF(ray, normal, hit, shadow, light);
}

Vector3 Shader::BRDF(const Ray& ray, const Vector3& normal, const Vector3& hit, const float shadow, const Light* light) const
{
	Vector3 Lo = 0.0f;
	const float pdf = 1.0f / (2.0f * PI);
	Size samples = light->Samples;
	for (Size i = 0; i < samples; ++i)
	{
		const auto viewDirection = (ray.GetOrigin() - hit).Normalized();
		const auto reflection = Ray::Reflection(normal, viewDirection);

		const auto lightSampleSpecular = light->Sampler(hit, reflection, normal, Roughness);
		const Vector3 lightDirection = lightSampleSpecular.IncomingRay.GetDirection();

		const Vector3 halfDirection = (viewDirection + lightDirection).Normalized();
		const float HdotV = halfDirection.DotProduct(viewDirection);
		const float NdotV = normal.DotProduct(viewDirection);
		const float NdotL = normal.DotProduct(lightDirection);

		const float distance = lightSampleSpecular.Distance;
		const float attenuation = 1.0f / (distance * distance);
		const Vector3 radiance = lightSampleSpecular.Colour * attenuation;

		Vector3 F0 = Vector3(0.04f).Mix(Albedo, Metalness);

		const auto NDF = Distribution(normal, halfDirection, Roughness);
		const auto G = Geometry(normal, viewDirection, lightDirection, Roughness);
		Vector3 F = Fresnel(std::max(HdotV, 0.0f), F0);

		const auto nominator = G * F * NDF;
		float denominator = 4.0f * std::max(NdotV, 0.0f) * std::max(NdotL, 0.0f) + 0.001f;
		Vector3 specular = nominator / std::max(denominator, 0.001f);

		Vector3 kS = F;
		Vector3 kD = Vector3(1.0f) - kS;
		kD *= 1.0f - Metalness;

		const auto lightSampleDiffuse = light->Sampler(hit, normal, reflection, 1.2f);
		Vector3 diffuse = lightSampleDiffuse.Colour * Albedo;

		Lo += (((kD * diffuse) / PI) + specular) * radiance * std::max(NdotL, 0.0f);
	}
	Lo = Lo * (1.0f / float(samples));// *pdf;

	return Lo;
}

Vector3 Shader::Fresnel(const float incidenceAngle, const Vector3& ior) const
{
	return ior + (Vector3(1.0f) - ior) * std::pow(1.0f - incidenceAngle, 5.0f);
}

Vector3 Shader::Geometry(const Vector3& normal, const Vector3& view, const Vector3& lightDirection, const float k) const
{
	auto lSchlickGGXD = [&](const float NDotV, const float roughness) -> float
	{
		//float a = roughness;
		//float k = (a * a) / 2.0;
		//k = roughness;
		return NDotV / (NDotV * (1.0f - k) + k);
	};

	// GeometrySmith
	float NdotV = std::max(normal.DotProduct(view), 0.0f);
	float NdotL = std::max(normal.DotProduct(lightDirection), 0.0f);
	float GGX1 = lSchlickGGXD(NdotV, k);
	float GGX2 = lSchlickGGXD(NdotL, k);
	return GGX1 * GGX2;
}

float Shader::Distribution(const Vector3 normal, const Vector3 half, const float roughness) const
{
	const float a = roughness * roughness;
	const float a2 = a * a;
	const float NdotH = std::max(normal.DotProduct(half), 0.0f);
	const float NdotH2 = NdotH * NdotH;

	const float nom = a2;
	float denom = NdotH2 * (a2 - 1.0f) + 1.0f;
	denom = PI * denom * denom;

	return nom / denom;
}