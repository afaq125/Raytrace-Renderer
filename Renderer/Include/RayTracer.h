#pragma once

namespace Renderer
{
	using namespace Math;
	using namespace Lights;

	class Scene
	{
	public:
		Scene(
			std::vector<std::shared_ptr<Object>> Objects,
			std::vector<std::shared_ptr<Light>> Lights,
			Camera camera = Camera(1024, 1024)) :
			Objects(std::move(Objects)),
			Lights(std::move(Lights)),
			Cam(camera)
		{ 
		}

		Scene() = default;
		~Scene() = default;
		Scene(const Scene&) = delete;
		Scene(Scene&&) = delete;
		Scene& operator=(const Scene& scene) = delete;

		std::vector<std::shared_ptr<Object>> Objects;
		std::vector<std::shared_ptr<Light>> Lights;
		Camera Cam = Camera(1024, 1024);
	};

	class RayTracer
	{
	public:
		struct Settings
		{
			Vector3 BackgroundColour = { 0.0f, 0.0f, 0.0f };
			Size SamplesPerPixel = 20u;
			Size MaxDepth = 2u;
			Size MaxGIDepth = 2u;
			Size SecondryBounces = 10u;
		};

		RayTracer() = delete;
		RayTracer(
			const Scene& scene,
			const RayTracer::Settings settings = RayTracer::Settings()) :
			mScene(scene),
			mCamera(scene.Cam),
			mSettings(settings)
		{
		}
		~RayTracer() = default;

        const Viewport& Render(
			const std::function<void(const Viewport::Pixels&, const std::string&)>& save = [](const  Viewport::Pixels& viewport, const std::string& path) -> void {},
			const std::string& path = "");

		Intersection Trace(const Ray& ray, const Size depth = 0u) const;

	private:
		Vector3 GlobalIllumination(const Ray& ray, const Vector3& normal, const Vector3& hit, const Size depth) const;

		const std::reference_wrapper<const Scene> mScene;
		Camera mCamera;
		const Settings mSettings;
	};
}