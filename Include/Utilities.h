#pragma once

namespace Renderer
{
	using namespace Math;

	class Object;

	class Transform
	{
	public:
		Transform() :
			mAxis(Matrix3()),
			mPosition(Vector3())
		{
			mAxis.Identity();
		}

		Transform(const Matrix3& axis, const Vector3& position) :
			mAxis(axis),
			mPosition(position)
		{
		}

		Transform(const Vector3& direction, const Vector3& up, const Vector3& position);

		~Transform() = default;

		void SetAxis(const Matrix3& axis) { mAxis = axis; }
		void SetPosition(const Vector3& position) { mPosition = position; }
		Matrix3 GetAxis() const { return mAxis; }
		Vector3 GetPosition() const { return mPosition; }

	private:
		Matrix3 mAxis;
		Vector3 mPosition;
	};

	class Ray
	{
	public:
		Ray(Vector3 origin, Vector3 direction) :
			mOrigin(std::move(origin)),
			mDirection(std::move(direction))
		{
			mDirection.Normalize();
		}
		~Ray() = default;

		const Vector3& GetOrigin() const { return mOrigin; }
		const Vector3& GetDirection() const { return mDirection; }
		Vector3 Projection(const Vector3& position) const;
		static Vector3 Reflection(const Vector3& normal, const Vector3& direction);

	private:
		Vector3 mOrigin;
		Vector3 mDirection;
	};

	struct Intersection
	{
		bool Hit = false;
		Vector3 Position = Vector3();
		Vector3 SurfaceColour = Vector3();
		const Object* Object = nullptr;
	};

	std::vector<Intersection> IntersectScene(const std::vector<std::shared_ptr<Object>>& objects, const Ray& ray, bool checkAll);
	float Random();
	Vector3 SampleHemisphere(const float r1, const float r2);
	Vector3 SampleCircle(const float r);
}