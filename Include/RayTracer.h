#pragma once

namespace Renderer
{
	using namespace Math;
	using namespace Lights;

	class RayTracer
	{
	public:
		RayTracer(
			const std::vector<std::shared_ptr<Object>>& objects, 
			const std::vector<std::shared_ptr<Light>>& lights,
			const Camera camera = Camera(1024, 1024)) :
			mObjects(objects),
			mLights(lights),
			mCamera(camera)
		{
			Initialise();
		}
		~RayTracer() = default;

		void Initialise();

		Camera::Viewport Render(
			const std::function<void(const Camera::Viewport&, const std::string&)>& save = [](const Camera::Viewport& viewport, const std::string& path) -> void {},
			const std::string& path = "");

		Intersection Trace(const Ray& ray, const Size depth = 0) const;

	private:
		Vector3 mBackgroundColour;
		Size mSamplesPerPixel;
		Size mMaxDepth;
		Size mMaxGIDepth;
		Size mSecondryBounces;

		const std::vector<std::shared_ptr<Object>>& mObjects;
		const std::vector<std::shared_ptr<Light>>& mLights;
		std::vector<std::shared_ptr<Object>> mRendableObjects;
		Camera mCamera;
	};
}