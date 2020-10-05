#pragma once

namespace Renderer
{
	using namespace Math;
	using namespace Lights;

	class RayTracer
	{
	public:
		struct Settings
		{
			Vector3 BackgroundColour = { 0.0f, 0.0f, 0.0f };
			Size SamplesPerPixel = 30u;
			Size MaxDepth = 1u;
			Size MaxGIDepth = 0u;
			Size SecondryBounces = 20u;
		};

		RayTracer() = delete;
		RayTracer(
			const std::vector<std::shared_ptr<Object>>& objects, 
			const std::vector<std::shared_ptr<Light>>& lights,
			const Camera camera = Camera(1024, 1024),
			const RayTracer::Settings settings = RayTracer::Settings()) :
			mObjects(objects),
			mLights(lights),
			mCamera(camera),
			mSettings(settings)
		{
			Initialise();
		}
		~RayTracer() = default;

		void Initialise();

		Camera::Viewport Render(
			const std::function<void(const Camera::Viewport&, const std::string&)>& save = [](const Camera::Viewport& viewport, const std::string& path) -> void {},
			const std::string& path = "");

		Intersection Trace(const Ray& ray, const Size depth = 0u) const;

	private:
		Vector3 GlobalIllumination(const Ray& ray, const Vector3& normal, const Vector3& hit, const Size depth) const;

		const std::vector<std::shared_ptr<Object>>& mObjects;
		const std::vector<std::shared_ptr<Light>>& mLights;
		Camera mCamera;
		const Settings mSettings;

		std::vector<std::shared_ptr<Object>> mRendableObjects;
	};
}