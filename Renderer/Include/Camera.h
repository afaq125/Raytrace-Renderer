#pragma once

namespace Renderer
{
	using namespace Math;

	class Camera
	{
	public:
		Camera(const Size width, const Size height, const float focalLength = 1.0f, const float pixel_spacing = 0.1f) :
			FocalLength(focalLength),
            m_viewport(width, height, pixel_spacing)
		{
		};
		~Camera() = default;

		void LookAt(const Vector3& target, const Vector3& up);
		Ray CreateRay(const Size pixel, const float randomMultiplier = 0.01f) const;

        Viewport& GetViewport() { return m_viewport; }

		float FocalLength;
		Transform XForm;

	private:
		Viewport m_viewport;
	};
}