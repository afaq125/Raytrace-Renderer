#pragma once

namespace Renderer
{
	using namespace Math;

	class Object;

	class Transform
	{
	public:
		Transform() :
            m_axis(Matrix3()),
			m_position(Vector3()),
            m_inverse(Matrix3())
		{
            m_axis.Identity();
            m_inverse.Identity();
		}

		Transform(const Matrix3& axis, const Vector3& position, const bool calculate_inverse) :
            m_axis(axis),
            m_position(position),
            m_inverse(Matrix3())
		{
            if (calculate_inverse)
            {
                m_inverse = m_axis.Inversed();
            }
		}

		Transform(const Vector3& direction, const Vector3& up, const Vector3& position, const bool calculate_inverse);

		~Transform() = default;

		void SetAxis(const Matrix3& axis, const bool calculate_inverse = true)
        {
            m_axis = axis;
            if (calculate_inverse)
            {
                m_inverse = m_axis.Inversed();
            }
        }
		void SetPosition(const Vector3& position) { m_position = position; }
		const Matrix3& GetAxis() const { return m_axis; }
		const Vector3& GetPosition() const { return m_position; }
        const Matrix<float>& GetInverse() const { return m_inverse; }

	private:
		Matrix3 m_axis;
		Vector3 m_position;
        Matrix<float> m_inverse;
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
	Vector3 ImportanceSampleHemisphereGGX(const float r1, const float r2, const float roughness);
	Vector3 SampleCircle(const float r);
}