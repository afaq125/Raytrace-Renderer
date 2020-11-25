#pragma once

namespace Renderer
{
	using namespace Math;

	class Camera
	{
	public:
		Camera(float width, float height, float focalLength = 1.0, float pixelSpacing = 0.1) :
			Width(width),
			Height(height),
			FocalLength(focalLength),
			PixelSpacing(pixelSpacing)
		{
			Initialise();
		};
		~Camera() = default;

		using Viewport = std::array<Matrix<float>, 3>;

		void SetAspectRatio(const float a, const float b);

		void SetPixel(const Size index, const float r, const float g, const float b);

		Vector3 GetPixelPosition(const float u, const float v) const;

		void LookAt(const Vector3& target, const Vector3& up);
		Vector2 PixelToUV(const Size index) const;

		Ray CreateRay(const Size pixel, const float randomMultiplier = 0.01f) const;

		float GetViewportArea() const { return static_cast<float>(mViewport[0].Area()); }
		const Viewport& GetViewport() const { return mViewport; }

		float Width;
		float Height;
		float FocalLength;
		float PixelSpacing;
		Transform XForm;

	private:
		void Initialise();

		Viewport mViewport;
	};
}