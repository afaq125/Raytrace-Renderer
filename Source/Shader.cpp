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

void Texture::SetPixel(const float u, const float v, const Vector3& rgb)
{
	const auto rows = static_cast<float>(Pixels[0].Rows());
	const auto columns = static_cast<float>(Pixels[0].Columns());
	const Size x = static_cast<Size>(u * rows);
	const Size y = static_cast<Size>(v * columns);
	Pixels[0].Set(x, y, rgb[0]);
	Pixels[1].Set(x, y, rgb[1]);
	Pixels[2].Set(x, y, rgb[2]);
}

Vector3 Shader::BSDF(
	const Ray& ray, 
	const Vector3& normal, 
	const Vector3& hit, 
	const std::vector<std::shared_ptr<Object>>& objects, 
	const std::vector<std::shared_ptr<Light>>& lights) const
{
	return BRDF(ray, normal, hit, objects, lights);
}

Vector3 Shader::BRDF(
	const Ray& ray, 
	const Vector3& normal, 
	const Vector3& hit, 
	const std::vector<std::shared_ptr<Object>>& objects, 
	const std::vector<std::shared_ptr<Light>>& lights) const
{

	const auto viewDirection = (ray.GetOrigin() - hit).Normalized();
	const auto reflection = Ray::Reflection(normal, viewDirection);
	const auto NdotV = normal.DotProduct(viewDirection);
	const auto F0 = Vector3(0.04f).Mix(Albedo, Metalness);
	
	const auto pdf = 1.0f / (2.0f * PI);
	Vector3 ambient = Albedo * Vector3(0.03f);
	std::shared_ptr<Enviroment> environment = nullptr;
	for (const auto& light : lights)
	{
		environment = std::dynamic_pointer_cast<Enviroment>(light);
		if (environment)
		{
			environment->SamplerType = Enviroment::Sampler::SAMPLE_HEMISPHERE;

			const auto F = Fresnel(std::max(NdotV, 0.0f), F0);
			const auto kS = F;
			auto kD = Vector3(1.0f) - kS;
			kD *= 1.0f - Metalness;

			Vector3 radiance = 0.0f;
			const Size samples = environment->Samples;
			for (Size i = 0; i < samples; ++i)
			{
				radiance += environment->Sampler(hit, normal, reflection, 1.0f).Colour;
			}
			radiance = radiance * (1.0f / float(samples)) * pdf;
			const auto diffuse = radiance * Albedo;
			ambient = (kD * diffuse) * 0.1f;

			environment->SamplerType = Enviroment::Sampler::SAMPLE_HEMISPHERE_GGX;
		}
	}

	const auto sceneReflections = SceneReflections(
		ray.GetOrigin(), hit, normal, Roughness, ReflectionDepth, ReflectionSamples, objects);

	Vector3 Lo = 0.0f;
	for (const auto& light : lights)
	{
		Vector3 L = 0.0f;
		Size samples = light->Samples;
		for (Size i = 0; i < samples; ++i)
		{
			const auto lightSampleSpecular = light->Sampler(hit, reflection, normal, Roughness);
			const auto lightDirection = lightSampleSpecular.IncomingRay.GetDirection();
			auto lightColour = lightSampleSpecular.Colour;

			const auto halfDirection = (viewDirection + lightDirection).Normalized();
			const auto HdotV = halfDirection.DotProduct(viewDirection);
			const auto NdotL = normal.DotProduct(lightDirection);

			if (environment)
			{
				lightColour += sceneReflections * 100.0f;
			}
			const auto radiance = light->Attenuation(lightColour, light->Intensity, lightSampleSpecular.Distance);

			const auto NDF = Distribution(normal, halfDirection, Roughness);
			const auto G = Geometry(normal, viewDirection, lightDirection, Roughness);
			const auto F = Fresnel(std::max(HdotV, 0.0f), F0);

			const auto nominator = F * NDF * G;
			const auto denominator = 4.0f * std::max(NdotV, 0.0f) * std::max(NdotL, 0.0f);
			const auto specular = nominator / std::max(denominator, 0.001f);

			const auto kS = F;
			auto kD = Vector3(1.0f) - kS;
			kD *= 1.0f - Metalness;

			L += (((kD * Albedo) / PI) + specular) * radiance * std::max(NdotL, 0.0f);
		}
		L = L * (1.0f / float(samples));
		Lo += L;
	}

	Vector3 colour = ambient + Lo;
	//colour.Clamp(0.0f, 1.0f);

	// HDR tonemapping
	colour = colour / (colour + Vector3(1.0));
	// Gamma correct
	colour.Pow(1.0f / 2.2f);
	return colour;
}

float Shader::Shadow(const Vector3& hit,
	const std::vector<std::shared_ptr<Object>>& objects,
	const std::vector<std::shared_ptr<Light>>& lights) const
{
	float shadow = 0.0f;
	for (const auto& light : lights)
	{
		shadow += light->Shadow(objects, hit);
	}
	shadow *= (1.0f / static_cast<float>(lights.size()));
	return shadow;
}

Vector3 Shader::SceneReflections(
	Vector3 origin,
	Vector3 hit,
	Vector3 normal,
	float roughness,
	const Size depth,
	const Size samples,
	const std::vector<std::shared_ptr<Object>>& objects) const
{
	auto colour = Vector3();
	for (Size i = 0; i < depth; ++i)
	{
		Intersection intersection;
		for (Size i = 0; i < samples; ++i)
		{
			const auto view = (origin - hit).Normalized();
			const auto reflection = Ray::Reflection(normal, view);
			const auto axis = Transform(reflection, view, hit);
			const float random1 = Random();
			const float random2 = Random();
			const Vector3 hemisphereSample = ImportanceSampleHemisphereGGX(random1, random2, roughness);
			const Vector3 hemisphereSampleToWorldSpace = hemisphereSample.MatrixMultiply(axis.GetAxis());
			const auto ray = Ray(hit, hemisphereSampleToWorldSpace);

			const auto intersections = IntersectScene(objects, ray, true);
			if (intersections.empty())
			{
				return Vector3();
			}

			intersection = intersections.front();
			colour += intersection.SurfaceColour;
		}
		const auto object = intersection.Object;
		origin = hit;
		hit = intersection.Position + (normal * 0.0001f);
		normal = object->CalculateNormal(intersection.Position);
		colour /= samples;
	}

	return colour;
};

Vector3 Shader::Fresnel(const float incidenceAngle, const Vector3& ior) const
{
	return ior + (Vector3(1.0f) - ior) * std::pow(1.0f - incidenceAngle, 5.0f);
}

float Shader::Geometry(const Vector3& normal, const Vector3& view, const Vector3& lightDirection, const float k) const
{
	auto lSchlickGGX = [=](const float NDotV, const float roughness) -> float
	{
		float r = roughness + 1.0f;
		float k = (r * r) / 8.0f;
		return NDotV / (NDotV * (1.0f - k) + k);
	};

	// GeometrySmith
	float NdotV = std::max(normal.DotProduct(view), 0.0f);
	float NdotL = std::max(normal.DotProduct(lightDirection), 0.0f);
	float GGX1 = lSchlickGGX(NdotV, k);
	float GGX2 = lSchlickGGX(NdotL, k);
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

	return nom / std::max(denom, 0.001f);
}