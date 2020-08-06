#pragma once

namespace Renderer
{
	using namespace Math;

	class Object
	{
	public:
		Object() :
			XForm(Transform()),
			Material(Shader())
		{}
		~Object() = default;

		virtual Vector3 CalculateNormal(const Vector3& hit) const = 0;
		virtual Intersection Intersect(const Ray& ray) const = 0;

		Transform XForm;
		Shader Material;
	};

	class Plane : public Object
	{
	public:
		Plane() : Object() {}
		Plane(const float width, const float height) :
			Object(),
			Width(width),
			Height(height)
		{
		}

		float Width = 10.0f;
		float Height = 10.0f;

		Vector3 UVToWorld(const float u, const float v) const;
		void SetDirection(const Vector3& direction);

		virtual Vector3 CalculateNormal(const Vector3& hit) const override;
		virtual Intersection Intersect(const Ray& ray) const override;
	};

	class Sphere : public Object
	{
	public:
		float Radius = 1.0f;

		Intersection Intersect(const Ray& ray) const override;
		Vector3 CalculateNormal(const Vector3& hit) const override;
	};
}