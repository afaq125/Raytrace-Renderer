#pragma once

namespace
{
	std::chrono::seconds CurrentTime()
	{
		auto now = std::chrono::system_clock::now();
		auto epoch = std::chrono::time_point_cast<std::chrono::seconds>(now).time_since_epoch();
		return std::chrono::duration_cast<std::chrono::seconds>(epoch);
	}

	std::default_random_engine Generator;
	std::uniform_real_distribution<float> Distribution(0, 1);
}

namespace Renderer
{
	using namespace Math;

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

		Transform(const Vector3& direction, const Vector3& up, const Vector3& position) :
			mAxis(Matrix3()),
			mPosition(position)
		{
			auto z = direction.Normalized();
			auto y = z.CrossProduct(up).Normalized();
			auto x = z.CrossProduct(y).Normalized();
			for (Size i = 0; i < 3; ++i)
			{
				mAxis.Set(i, 0, y[i]);
				mAxis.Set(i, 1, z[i]);
				mAxis.Set(i, 2, x[i]);
			}
		}

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
		Vector3 Projection(const Vector3& position) const
		{
			const float distance = (position - mOrigin).DotProduct(mDirection);
			return mOrigin + (mDirection * distance);
		}
		static Vector3 Reflection(const Vector3& normal, const Vector3& direction)
		{
			return (((normal - direction) * normal.DotProduct(direction)) * 2.0);
		}

	private:
		Vector3 mOrigin;
		Vector3 mDirection;
	};

	class Shader
	{
	public:
		Shader() = default;
		~Shader() = default;

		Vector3 Colour = { 1.0, 0.0, 0.0 };
		bool Reflective = false;
	};

	struct Grid
	{
		float Scale = 10.0;
		Vector3 Position = { 0.0, 0.0, 0.0 };
	};

	class Object;

	struct Intersection
	{
		bool Hit = false;
		Vector3 Position = Vector3();
		Vector3 SurfaceColour = Vector3();
		const Object* Object = nullptr;
	};

	class Object
	{
	public:
		Object() = default;
		~Object() = default;

		virtual Vector3 CalculateNormal(const Vector3& hit) const = 0;
		virtual Intersection Intersect(const Ray& ray) const = 0;

		Transform XForm;
		Shader Material;
	};

	class Plane : public Object
	{
	public:
		float Width = 5.0;
		float Height = 5.0;

		virtual Vector3 CalculateNormal(const Vector3& hit) const override
		{
			return Y_AXIS;
		}

		virtual Intersection Intersect(const Ray& ray) const override
		{
			auto projection = ray.Projection(XForm.GetPosition());
			const double distance = (projection - XForm.GetPosition()).Length();

			if (projection[1] < 0)
			{
				return Intersection();
			}

			//projection = projection.MatrixMultiply(Transform(Y_AXIS, X_AXIS, XForm.GetPosition()).GetAxis());

			return { true,  projection, Material.Colour, static_cast<const Object*>(this) };
		}
	};

	class Sphere : public Object
	{
	public:
		float Radius = 1.0;

		Intersection Intersect(const Ray& ray) const override
		{
			const auto sphereToRay = XForm.GetPosition() - ray.GetOrigin();
			const auto sign = sphereToRay.DotProduct(ray.GetDirection());

			if (sign < 0.0)
			{
				if (sphereToRay.Length() > Radius)
				{
					return Intersection();
				}
				if (sphereToRay.Length() == Radius)
				{
					// TODO: Check this but it should be the case when the ray origin is on the edge of the sphere.
					return { true, ray.GetOrigin(), Material.Colour, static_cast<const Object*>(this) };
				}
			}

			const Vector3 projection = ray.Projection(XForm.GetPosition());
			if ((XForm.GetPosition() - projection).Length() > Radius)
			{
				return Intersection();
			}

			const Vector3 projMinusPos = projection - XForm.GetPosition();
			const float distance = std::sqrt((Radius * Radius) - (projMinusPos.Length() * projMinusPos.Length()));

			float offset = (projection - ray.GetOrigin()).Length();
			if (sphereToRay.Length() > Radius)
			{
				offset -= distance;
			}
			else
			{
				offset += distance;
			}

			return { true, ray.GetOrigin() + (ray.GetDirection() * offset), Material.Colour, static_cast<const Object*>(this) };
		}

		Vector3 CalculateNormal(const Vector3& hit) const override
		{
			return (hit - XForm.GetPosition()).Normalized();
		}
	};

	struct Light
	{
		Vector3 Position = { 0.0, 10.0, 0.0 };
	};

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

		void SetAspectRatio(const float a, const float b)
		{
			const float ratio = a / b;
			Height /= ratio;
			Initialise();
		}

		void SetPixel(const Size index, const float r, const float g, const float b)
		{
			mViewport[0][index] = r;
			mViewport[1][index] = g;
			mViewport[2][index] = b;
		}

		Vector3 GetPixelPosition(const float u, const float v) const
		{
			const float hWidth = Width / 2.0f;
			const float hHeight = Height / 2.0f;

			const float x = ((Width * u) - hWidth) * PixelSpacing;
			const float z = ((Height * v) - hHeight) * PixelSpacing;

			Vector3 position = { x, -FocalLength, z };
			auto transformed = position.MatrixMultiply(XForm.GetAxis());
			transformed += XForm.GetPosition();

			return transformed;
		}

		void LookAt(const Vector3& target, const Vector3& up)
		{
			const auto direction = XForm.GetPosition() - target;
			const auto axis = Transform(direction, up, XForm.GetPosition());
			XForm.SetAxis(axis.GetAxis());
		}

		Vector2 PixelToUV(const Size index) const
		{
			const float column = static_cast<float>(mViewport[0].ColumnNumberFromIndex(index));
			const float row = static_cast<float>(mViewport[0].RowNumberFromIndex(index));
			const float u = column / static_cast<float>(mViewport[0].Columns());
			const float v = row / static_cast<float>(mViewport[0].Rows());
			return { u, v };
		}

		Ray CreateRay(const Size pixel, const float randomMultiplier = 0.01f) const
		{
			const auto uv = PixelToUV(pixel);
			const auto position = GetPixelPosition(uv[0], uv[1]);

			const float r1 = Distribution(Generator) - 0.5f;
			const float r2 = Distribution(Generator) - 0.5f;
			const float r3 = Distribution(Generator) - 0.5f;

			const float rx = (position - XForm.GetPosition())[0] + (r1 * randomMultiplier);
			const float ry = (position - XForm.GetPosition())[1] + (r2 * randomMultiplier);
			const float rz = (position - XForm.GetPosition())[2] + (r3 * randomMultiplier);

			const Vector3 origin = XForm.GetPosition();
			const Vector3 direction = { rx, ry, rz };
			return Ray(origin, direction);
		}

		float GetViewportArea() const { return static_cast<float>(mViewport[0].Area()); }
		const Viewport& GetViewport() const { return mViewport; }

		float Width;
		float Height;
		float FocalLength;
		float PixelSpacing;
		Transform XForm;

	private:
		void Initialise()
		{
			mViewport[0] = Matrix<float>(0.0, static_cast<Size>(Height), static_cast<Size>(Width));
			mViewport[1] = Matrix<float>(0.0, static_cast<Size>(Height), static_cast<Size>(Width));
			mViewport[2] = Matrix<float>(0.0, static_cast<Size>(Height), static_cast<Size>(Width));
		}

		Viewport mViewport;
	};

	class RayTracer
	{
	public:
		RayTracer(const std::vector<std::shared_ptr<Object>>& objects, const Camera camera = Camera(1024, 1024)) :
			mObjects(objects),
			mCamera(camera)
		{
			Initialise();
		}
		~RayTracer() = default;

		void Initialise()
		{
			mLight.Position = { 8.0, 8.0, 8.0 };

			mBackgroundColour = { 0.0, 0.0, 0.0 };
			mSamplesPerPixel = 2u;
			mMaxDepth = 1u;
			mMaxGIDepth = 1u;
			mSecondryBounces = 1u;
		}

		Camera::Viewport Render(
			const std::function<void(const Camera::Viewport&, const std::string&)>& save = [](const Camera::Viewport& viewport, const std::string& path) -> void {},
			const std::string& path = "")
		{
			auto job = [&](const Size i) -> void
			{
				auto colour = Vector3();
				for (Size s = 0; s < mSamplesPerPixel; ++s)
				{
					const auto ray = mCamera.CreateRay(i);
					const auto raytrace = Trace(ray, i);
					colour += raytrace.SurfaceColour;
				}
				colour *= 1.0f / static_cast<float>(mSamplesPerPixel);
				colour.Clamp(0.0f, 0.9999f);

				mCamera.SetPixel(i, colour[0], colour[1], colour[2]);
			};

			const auto start = CurrentTime();

			// Render
			const Size buckets = static_cast<Size>(mCamera.GetViewportArea());
			ThreadPool::RunWithCallback(job, [&]() { save(mCamera.GetViewport(), path); std::this_thread::sleep_for(std::chrono::seconds(10)); }, buckets);

			const auto end = CurrentTime();
			std::cout << "Start: " << start.count() << std::endl;
			std::cout << "End: " << end.count() << std::endl;
			std::cout << "Taken: " << (end.count() - start.count()) / 60.0 << std::endl;

			return mCamera.GetViewport();
		}

		Intersection Trace(const Ray& ray, const Size pixel, const Size depth = 0)
		{
			auto intersections = IntersectScene(ray, true);

			if (intersections.empty())
				return { false, Vector3(), mBackgroundColour, nullptr };

			auto illumination = Vector3();
			auto intersection = intersections.front();
			const auto object = intersection.Object;
			const auto normal = object->CalculateNormal(intersection.Position);
			const auto hit = intersection.Position + (normal * 0.0001f);
			float shadow = 1.0f;

			if (Shadow(Ray(hit, mLight.Position - hit)))
				shadow = 0.5f;

			if (object->Material.Reflective && depth < mMaxDepth)
			{
				const auto reflectionRay = Ray(hit, Ray::Reflection(normal, ray.GetOrigin() - hit));
				auto reflection = Trace(reflectionRay, pixel, depth + 1).SurfaceColour * 1.0f;

				float facingRatio = -ray.GetDirection().DotProduct(hit);
				float fresnel = mix(std::pow(1.0f - facingRatio, 3), 1.0f, 0.1f);

				illumination = (reflection * fresnel) * object->Material.Colour;
			}
			else
			{
				const auto lightDirection = (mLight.Position - hit).Normalized();
				const float lambertian = std::max(normal.DotProduct(lightDirection), 0.2f);
				const auto diffuse = object->Material.Colour * lambertian;
				const auto specular = Phong(ray.GetDirection(), lightDirection, normal); (diffuse + specular) * shadow;
				const auto direct = (diffuse) * shadow;

				Vector3 indirect = 0.0f;
				if (depth < mMaxGIDepth)
				{
					const float pdf = 1.0f / (2.0f * PI);
					const auto axis = Transform(normal, (ray.GetOrigin() - hit).Normalized(), hit);
					for (Size i = 0; i < mSecondryBounces; ++i)
					{
						const float r1 = Distribution(Generator);
						const float r2 = Distribution(Generator);

						const Vector3 hemisphereSample = SampleHemisphere(r1, r2);
						const Vector3 hemisphereSampleToWorldSpace = hemisphereSample.MatrixMultiply(axis.GetAxis());
						const Ray indirectRay(axis.GetPosition(), hemisphereSampleToWorldSpace);

						auto giIntersection = Trace(indirectRay, pixel, depth + 1);
						auto colour = (giIntersection.SurfaceColour);// *r1) / pdf;
						colour.SetNaNsOrINFs(0.0);
						indirect += colour;
					}
					indirect /= static_cast<float>(mSecondryBounces);
					indirect /= static_cast<float>(depth + 1);
				}

				illumination = (direct / PI) + (indirect * 2.0);
			}

			illumination.Clamp(0.0, 1.0);
			intersection.SurfaceColour = illumination;
			return intersection;
		}

		std::vector<Intersection> IntersectScene(const Ray& ray, bool checkAll) const
		{
			std::vector<Intersection> intersections;
			float distance = Infinity;
			for (const auto& o : mObjects)
			{
				Intersection intersect = o->Intersect(ray);
				if (intersect.Hit)
				{
					if (!checkAll)
					{
						return { intersect };
					}

					intersections.push_back(intersect);
					const float hitDistance = ray.GetOrigin().Distance(intersect.Position);
					if (hitDistance < distance)
					{
						distance = hitDistance;
						// Only care if front element is the closest;
						std::swap(intersections.front(), intersections.back());
					}
				}
			}
			return intersections;
		}

		bool Shadow(const Ray& ray) const
		{
			return !IntersectScene(ray, false).empty();
		}

		float Phong(const Vector3& view, const Vector3& lightDirection, const Vector3& normal) const
		{
			const auto reflection = Ray::Reflection(normal, lightDirection) * -1.0f;
			auto specularAngle = std::max(reflection.DotProduct(view), 0.0f);
			auto specular = std::pow(specularAngle, 4.0f); // 2.0 default
			return specular;
		}

		float mix(const float &a, const float &b, const float &mix) const
		{
			return b * mix + a * (1 - mix);
		}

		Vector3 SampleHemisphere(const float r1, const float r2) const
		{
			float angle = std::sqrt(1.0f - (r1 * r1));
			float phi = 2.0f * PI * r2;
			float x = angle * std::cos(phi);
			float z = angle * std::sin(phi);
			return { x, r1, z };
		}

	private:
		Vector3 mBackgroundColour;
		Size mSamplesPerPixel;
		Size mMaxDepth;
		Size mMaxGIDepth;
		Size mSecondryBounces;

		const std::vector<std::shared_ptr<Object>>& mObjects;
		Light mLight;
		Camera mCamera;
	};
}