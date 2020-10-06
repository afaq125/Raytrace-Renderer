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
		Plane(const float width, 
			const float height, 
			const Vector3& position = {0.0f, 0.0f, 0.0f}, 
			const Vector3& direction = { 0.0f, 1.0f, 0.0f }) :
			Object(),
			Width(width),
			Height(height)
		{
			XForm.SetPosition(position);
			SetDirection(direction);
		}
		virtual ~Plane() {}

		float Width = 10.0f;
		float Height = 10.0f;

		Vector3 WorldToUV(const Vector3 position) const;
		Vector3 UVToWorld(const float u, const float v, const float surfaceOffset = 0.0f) const;
		void SetDirection(const Vector3& direction);

		virtual Vector3 CalculateNormal(const Vector3& hit) const override;
		virtual Intersection Intersect(const Ray& ray) const override;
	};

	class Sphere : public Object
	{
	public:
		Sphere() = default;
		virtual ~Sphere() {}

		float Radius = 1.0f;

		Intersection Intersect(const Ray& ray) const override;
		Vector3 CalculateNormal(const Vector3& hit) const override;
	};

	class Cube : public Object
	{
	public:
		Cube() = default;
		virtual ~Cube() {}

		float Width = 1.0f;
		float Height = 1.0f;
		float Length = 1.0f;

		Intersection Intersect(const Ray& ray) const override;
		Vector3 CalculateNormal(const Vector3& hit) const override;
	};
}