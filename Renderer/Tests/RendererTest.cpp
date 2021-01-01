#include "Tests.h"

using namespace Renderer;
using namespace Renderer::Math;

TEST_F(RendererUnitTests, AreaLightTest)
{
	std::vector<std::shared_ptr<Object>> objects;
	{
		const std::vector<Vector3> colours = {
			{ 68.0f, 75.0f, 91.0f },
			{ 205.0f, 90.0f, 87.0f },
			{ 120.0f, 164.0f, 163.0f },
			{ 225.0f, 177.0f, 105.0f },
			{ 225.0f, 177.0f, 105.0f },
			{ 170.0f, 170.0f, 170.0f }
		};

		objects.emplace_back(std::make_shared<Plane>(Plane(10.0f, 10.0f, { 5.0f, 0.0f, 0.0f },  { -1.0f, 0.0f, 0.0f })));
		objects.emplace_back(std::make_shared<Plane>(Plane(10.0f, 10.0f, { 0.0f, 5.0f, 0.0f },  { 0.0f, -1.0f, 0.0f })));
		objects.emplace_back(std::make_shared<Plane>(Plane(10.0f, 10.0f, { -5.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f })));
		objects.emplace_back(std::make_shared<Plane>(Plane(10.0f, 10.0f, { 0.0f, -5.0f, 0.0f }, { 0.0f, 1.0f, 0.0f })));

		for (auto& object : objects)
		{
			auto plane = std::static_pointer_cast<Plane>(object);
			plane->Material.Albedo = { 1.0f, 1.0f, 1.0f };
			plane->Material.Metalness = 0.0f;
			plane->Material.Roughness = 1.0f;
			plane->Material.ReflectionSamples = 0u;
			plane->Material.ReflectionDepth = 0u;
		}

		constexpr Size count = 10;
		for (Size i = 0; i < 4; ++i)
		{
			auto plane = std::static_pointer_cast<Plane>(objects[i]);

			for (Size j = 0; j < count; ++j)
			{
				const int randomIndex = static_cast<int>(Random() * static_cast<float>(colours.size()));
				const auto position = plane->UVToWorld(Random(), Random());
				const float radius = Random() * 2.0f;
				const Vector3 colour = (colours[randomIndex] / 255.0f) * 1.25f;

				auto sphere = std::make_shared<Sphere>();
				sphere->Radius = radius;
				sphere->XForm.SetPosition(position);
				sphere->Material.Albedo = colour;
				sphere->Material.Metalness = 1.0f;
				sphere->Material.Roughness = 0.5f;
				
				objects.push_back(sphere);
			}
		}

		for (Size i = 0; i < 4; ++i)
		{
			auto plane = std::static_pointer_cast<Plane>(objects[i]);
			plane->Width = 20.0f;
			plane->Height = 20.0f;
		}
	}

	std::vector<std::shared_ptr<Light>> lights;
	{
		auto aLight = std::make_shared<Lights::Area>();
		aLight->Intensity = 28.0f;
		aLight->Samples = 32;
		aLight->Colour = { 1.0f, 1.0f, 1.0f };
		aLight->ShadowIntensity = 0.5f;
		aLight->Grid->Width = 10.0f;
		aLight->Grid->Height = 10.0f;
		aLight->Grid->XForm.SetPosition({ 0.0f, 0.0f, -9.9f });
		aLight->Grid->SetDirection({ 0.0f, 0.0f, 1.0f });

		lights.push_back(aLight);
	}

	auto camera = Camera(1024.0f, 1024.0f, 1.5f, 0.005f);
	camera.XForm.SetPosition({ 0.0f, 0.0f, 9.0f });
	camera.LookAt({ 0.0f, 0.0f, 0.0f }, Y_MINUS_AXIS);

	RayTracer::Settings settings;
	settings.SamplesPerPixel = 20u;
	settings.MaxDepth = 1u;
	settings.MaxGIDepth = 0u;
	settings.SecondryBounces = 0u;

	const auto RenderGIScene = RayTracer(Scene(objects, lights, camera), settings).Render(&SaveImage, "Render_Update.png");
	SaveImage(RenderGIScene, "Render_AreaLight.png");
}

TEST_F(RendererUnitTests, GlobalIlluminationTest)
{
	std::vector<std::shared_ptr<Object>> objects;
	{
		const auto WH = 10.0f;
		const auto halfWH = WH / 2.0f;

		// Outter walls
		objects.emplace_back(std::make_shared<Plane>(Plane(WH, WH, { 0.0f, halfWH, 0.0f }, { 0.0f, -1.0f, 0.0f })));
		objects.emplace_back(std::make_shared<Plane>(Plane(WH, WH, { 0.0f, -halfWH, 0.0f }, { 0.0f, 1.0f, 0.0f })));
		objects.emplace_back(std::make_shared<Plane>(Plane(WH, WH, { halfWH, 0.0f, 0.0f }, { -1.0f, 0.0f, 0.0f })));
		objects.emplace_back(std::make_shared<Plane>(Plane(WH, WH, { -halfWH, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f })));
		objects.emplace_back(std::make_shared<Plane>(Plane(WH, WH, { 0.0f, 0.0f, -halfWH }, { 0.0f, 0.0f, 1.0f })));
		objects.emplace_back(std::make_shared<Plane>(Plane(WH, WH, { 0.0f, 0.0f, halfWH }, { 0.0f, 0.0f, -1.0f })));

		// Interior walls
		objects.emplace_back(std::make_shared<Plane>(Plane(WH + 0.1f, WH + 0.1f, { 0.0f, 1.0f, -0.25f }, { 0.0f, 0.0f, -1.0f })));
		objects.emplace_back(std::make_shared<Plane>(Plane(WH + 0.1f, WH + 0.1f, { 0.0f, 1.0f, 0.25f }, { 0.0f, 0.0f, 1.0f })));
		objects.emplace_back(std::make_shared<Plane>(Plane(WH + 0.1f, 0.5f, { 0.0f, -4.0f, 0.0f }, { 0.0f, -1.0f, 0.0f })));

		for (auto& object : objects)
		{
			auto plane = std::static_pointer_cast<Plane>(object);
			plane->Material.Albedo = { 1.0f, 1.0f, 1.0f };
			plane->Material.Metalness = 0.0f;
			plane->Material.Roughness = 1.0f;
			plane->Material.ReflectionSamples = 0u;
			plane->Material.ReflectionDepth = 0u;
		}

		const std::vector<Vector3> colours = {
			{ 1.0f, 0.0f, 0.0f },
			{ 0.0f, 1.0f, 0.0f },
			{ 0.0f, 0.0f, 1.0f },
		};

		for (Size i = 0; i < 3; ++i)
		{
			auto cube = std::make_shared<Cube>();
			cube->XForm.SetPosition({ -2.0f + static_cast<float>(i * 2), -4.5f, 1.5f });
			cube->Material.Albedo = colours[i];
			cube->Material.Metalness = 1.0f;
			cube->Material.Roughness = 0.1f;

			objects.push_back(cube);
		}
	}

	std::vector<std::shared_ptr<Light>> lights;
	{
		auto pLight = std::make_shared<Lights::Point>();
		pLight->XForm.SetPosition({ 0.0f, -2.25f, -4.5f });
		pLight->Intensity = 6.0f;
		pLight->Colour = { 0.9f, 0.9f, 0.9f };
		pLight->ShadowIntensity = 1.0f;

		lights.push_back(pLight);
	}

	auto camera = Camera(1024.0f, 1024.0f, 1.0f, 0.005f);
	camera.XForm.SetPosition({ 0.0f, -2.5f, 4.5f });
	camera.LookAt({ 0.0f, -2.5f, 0.0f }, Y_MINUS_AXIS);

	RayTracer::Settings settings;
	settings.SamplesPerPixel = 20u;
	settings.MaxDepth = 3u;
	settings.MaxGIDepth = 2u;
	settings.SecondryBounces = 15u;

	const auto RenderGIScene = RayTracer(Scene(objects, lights, camera), settings).Render(&SaveImage, "Render_Update.png");
	SaveImage(RenderGIScene, "Render_GI.png");
}

TEST_F(RendererUnitTests, PBRTest)
{
	std::vector<std::shared_ptr<Object>> objects;
	{
		const Size rows = 5;
		const Size columns = 5;
		const Size area = rows * columns;
		const float spacing = 5.0f;
		for (Size i = 0; i < area; ++i)
		{
			Size c = i / columns;
			Size r = i % rows;
			float x = (static_cast<float>(c) * spacing) - ((static_cast<float>(columns) * spacing) / 2.0f) + (spacing / 2.0f);
			float y = (static_cast<float>(r) * spacing) - ((static_cast<float>(rows) * spacing) / 2.0f) + (spacing / 2.0f);
			float z = 0.0f;
			Vector3 position = { x, y, z };

			auto sphere = std::make_shared<Sphere>();
			sphere->Radius = 2.0f;
			sphere->XForm.SetPosition(position);
			sphere->Material.Albedo = { 1.0f, 0.0f, 0.0f };
			sphere->Material.Metalness = Clamp((1.0f / rows) * r, 0.0f, 1.0f);
			sphere->Material.Roughness = Clamp((1.0f / columns) * c, 0.05f, 1.0f);

			objects.push_back(sphere);
		}

		auto plane = std::make_shared<Plane>();
		plane->Width = 1000.0f;
		plane->Height = 1000.0f;
		plane->SetDirection({ 0.0f, 0.0f, 1.0f });
		plane->XForm.SetPosition({ 0.0f, 0.0f, 0.0f });
		plane->Material.Albedo = { 0.8f, 0.8f, 0.8f };
		plane->Material.Metalness = 0.0f;
		plane->Material.Roughness = 1.0f;
		plane->Material.ReflectionSamples = 0u;
		plane->Material.ReflectionDepth = 0u;

		objects.push_back(plane);
	}

	std::vector<std::shared_ptr<Light>> lights;
	{
		for (Size i = 0; i < 4; ++i)
		{
			auto pLight = std::make_shared<Lights::Point>();
			pLight->Intensity = 18.0f;
			pLight->Colour = { 0.9f, 0.9f, 0.9f };
			pLight->ShadowIntensity = 0.8f;

			lights.push_back(pLight);
		}

		std::static_pointer_cast<Point>(lights[0])->XForm.SetPosition({ 10.0f, 10.0f, 10.0f });
		std::static_pointer_cast<Point>(lights[1])->XForm.SetPosition({ -10.0f, 10.0f, 10.0f });
		std::static_pointer_cast<Point>(lights[2])->XForm.SetPosition({ 10.0f, -10.0f, 10.0f });
		std::static_pointer_cast<Point>(lights[3])->XForm.SetPosition({ -10.0f, -10.0f, 10.0f });

		auto evLight = std::make_shared<Lights::Enviroment>(
			std::move(LoadImage("..\\..\\Assets\\EnviromentMaps\\Sky\\Top.png")),
			std::move(LoadImage("..\\..\\Assets\\EnviromentMaps\\Sky\\Bottom.png")),
			std::move(LoadImage("..\\..\\Assets\\EnviromentMaps\\Sky\\Left.png")),
			std::move(LoadImage("..\\..\\Assets\\EnviromentMaps\\Sky\\Right.png")),
			std::move(LoadImage("..\\..\\Assets\\EnviromentMaps\\Sky\\Back.png")),
			std::move(LoadImage("..\\..\\Assets\\EnviromentMaps\\Sky\\Front.png")));
		evLight->Intensity = 2.0f;

		lights.push_back(evLight);
	}

	auto camera = Camera(512.0f, 512.0f, 1.5f, 0.005f);
	camera.XForm.SetPosition({ 0.0f, 0.0f, 25.0f });
	camera.LookAt({ 0.0f, 0.0f, 0.0f }, Y_MINUS_AXIS);

	RayTracer::Settings settings;
	settings.SamplesPerPixel = 20u;
	settings.MaxDepth = 2u;
	settings.MaxGIDepth = 1u;
	settings.SecondryBounces = 10u;

	const auto RenderPBRScene = RayTracer(Scene(objects, lights, camera), settings).Render(&SaveImage, "Render_Update.png");
	SaveImage(RenderPBRScene, "Render_PBR.png");
}

TEST_F(RendererUnitTests, SpheresTest)
{
	Scene scene;

	const auto ReadFile = [](const std::string& path) -> std::vector<std::shared_ptr<Object>>
	{
		std::vector<std::shared_ptr<Object>> spheres;

		std::ifstream file(path, std::ifstream::in);
		if (file.is_open())
		{
			std::string line;
			while (getline(file, line))
			{
				std::vector<float> values;
				std::stringstream ss(line);
				for (float i; ss >> i;) 
				{
					values.push_back(i);
					if (ss.peek() == ',')
					{
						ss.ignore();
					}
				}

				if (values.size() != 10)
				{
					throw std::logic_error("Line does not contain correct number of arguments.");
				}

				auto sphere = std::make_shared<Sphere>();
				sphere->Radius = values[0];
				sphere->XForm.SetPosition({ values[1], values[2], values[3] });
				sphere->Material.Albedo = { values[4], values[5], values[6] };
				sphere->Material.Metalness = values[7];
				sphere->Material.Roughness = values[8];
				sphere->Material.ReflectionDepth = static_cast<Size>(values[9]);
				spheres.push_back(sphere);
			}
			file.close();
		}

		return spheres;
	};
	const auto WriteFile = [](const std::string& path, const Sphere& sphere) -> bool
	{
		std::ofstream file(path, std::ios::app);
		if (file.is_open())
		{
			file << sphere.Radius << ","
			     << sphere.XForm.GetPosition()[0] << ","
			     << sphere.XForm.GetPosition()[1] << ","
			     << sphere.XForm.GetPosition()[2] << ","
			     << sphere.Material.Albedo[0] << ","
			     << sphere.Material.Albedo[1] << ","
			     << sphere.Material.Albedo[2] << ","
			     << sphere.Material.Metalness << ","
				 << sphere.Material.Roughness << ","
			     << sphere.Material.ReflectionDepth
			     << "\n";
			file.close();
			return true;
		}
		return false;
	};
	const auto CreateSpheres = []() -> std::vector<std::shared_ptr<Object>>
	{
		Size i = 0;
		constexpr Size limit = 2000;
		constexpr Size count = 10;
		constexpr float scale = 10.0f;
		std::vector<std::shared_ptr<Object>> spheres;
		std::vector<Vector3> colours = {
			{ 68.0f, 75.0f, 91.0f },
			{ 205.0f, 90.0f, 87.0f },
			{ 120.0f, 164.0f, 163.0f },
			{ 225.0f, 177.0f, 105.0f },
			{ 225.0f, 177.0f, 105.0f },
			{ 170.0f, 170.0f, 170.0f }
		};

		while(spheres.size() < count || i < limit)
		{
			const float invert = i % 2 ? -1.0f : 1.0f;
			const auto hemisphereSample = SampleHemisphere(Random(), Random());
			
			auto radius = Random() * (scale * 0.1f);
			auto position = hemisphereSample * (scale + radius);

			const auto Exists = [&]() -> bool
			{
				const auto ray = Ray(Vector3(), hemisphereSample);
				for (const auto& sphere : spheres)
				{
					const auto intersection = sphere->Intersect(ray);
					if (intersection.Hit)
					{
						return true;
					}
					const float distance = position.Distance(sphere->XForm.GetPosition());
					const float difference = std::static_pointer_cast<Sphere>(sphere)->Radius + radius;
					if (difference > distance)
					{
						return true;
					}
				}
				return false;
			};

			if (Exists())
			{
				continue;
			}

			const int randomIndex = static_cast<int>(Random() * static_cast<float>(colours.size()));

			auto sphere = std::make_shared<Sphere>();
			sphere->Radius = radius;
			sphere->XForm.SetPosition(position);
			sphere->Material.Albedo = (colours[randomIndex] / 255.0f) * 1.25f;
			sphere->Material.Metalness = i % 2 ? 1.0f : 0.0f;
			sphere->Material.Roughness = 0.05f;
			sphere->Material.ReflectionDepth = 4u;
			spheres.push_back(sphere);

			++i;
		}

		auto sphere = std::make_shared<Sphere>();
		sphere->Radius = scale;
		sphere->XForm.SetPosition(Vector3());
		sphere->Material.Albedo = { 0.5f, 0.5f, 0.5f };
		sphere->Material.Metalness = 1.0f;
		sphere->Material.Roughness = 0.05f;
		sphere->Material.ReflectionDepth = 4u;
		spheres.push_back(sphere);

		std::swap(spheres.front(), spheres.back());

		return spheres;
	};

	std::vector<std::shared_ptr<Object>> objects;
	{
		const std::string file = "./spheres.txt";
		objects = ReadFile(file);
		if (objects.empty())
		{
			objects = CreateSpheres();
			for (const auto object : scene.Objects)
			{
				const auto sphere = std::static_pointer_cast<Sphere>(object);
				WriteFile(file, *sphere.get());
			}
		}
	}

	std::vector<std::shared_ptr<Light>> lights;
	{
		auto evLight = std::make_shared<Lights::Enviroment>(
			std::move(LoadImage("..\\..\\Assets\\EnviromentMaps\\Sky\\Top.png")),
			std::move(LoadImage("..\\..\\Assets\\EnviromentMaps\\Sky\\Bottom.png")),
			std::move(LoadImage("..\\..\\Assets\\EnviromentMaps\\Sky\\Left.png")),
			std::move(LoadImage("..\\..\\Assets\\EnviromentMaps\\Sky\\Right.png")),
			std::move(LoadImage("..\\..\\Assets\\EnviromentMaps\\Sky\\Back.png")),
			std::move(LoadImage("..\\..\\Assets\\EnviromentMaps\\Sky\\Front.png")));
		evLight->Intensity = 10.0f;
	
		lights.push_back(evLight);
	}

	auto camera = Camera(1024.0f, 1024.0f, 1.5f, 0.005f);
	camera.XForm.SetPosition({ 0.0f, 15.0f, 0.0f });
	camera.LookAt({ 0.0f, 0.0f, 0.0f }, Z_MINUS_AXIS);

	RayTracer::Settings settings;
	settings.SamplesPerPixel = 40u;
	settings.MaxDepth = 1u;
	settings.MaxGIDepth = 0u;
	settings.SecondryBounces = 0u;

	const auto RenderPBRScene = RayTracer(Scene(objects, lights, camera), settings).Render(&SaveImage, "Render_Update.png");
	SaveImage(RenderPBRScene, "Render_Spheres.png");
}

TEST_F(RendererUnitTests, CubesTest)
{
	std::vector<std::shared_ptr<Object>> objects;
	{
		auto plane = std::make_shared<Plane>();
		plane->Width = 1000.0f;
		plane->Height = 1000.0f;
		plane->XForm.SetPosition({ 0.0f, 0.0f, 0.0f });
		plane->SetDirection({ 0.0f, 1.0f, 0.0f });
		plane->Material.Albedo = { 1.0f, 1.0f, 1.0f };
		plane->Material.Metalness = 0.0f;
		plane->Material.Roughness = 1.0f;
		plane->Material.ReflectionSamples = 0u;
		plane->Material.ReflectionDepth = 0u;

		objects.push_back(plane);

		const auto map = (LoadImage("..\\..\\Assets\\Height_Map_128.png").Pixels[0] * 1.0f) + 1.0f;
		const float gWidth = 20.0f;
		const float gHeight = 20.0f;
		const auto rows = map.Rows();
		const auto columns = map.Columns();
		for (unsigned int c = 0; c < columns; ++c)
		{
			for (unsigned int r = 0; r < rows; ++r)
			{
				const float x = (static_cast<float>(c) * (gWidth / static_cast<float>(columns))) - gWidth;
				const float y = map.Get(c, r) / 2.0f;
				const float z = (static_cast<float>(r) * (gHeight / static_cast<float>(rows))) - gHeight;

				auto cube = std::make_shared<Cube>();
				cube->Width = (gWidth / static_cast<float>(columns)) - 0.01f;
				cube->Height = map.Get(c, r);
				cube->Length = (gHeight / static_cast<float>(rows)) - 0.01f;
				cube->XForm.SetPosition({ x, y, z });
				cube->Material.Albedo = { 0.988235f, 0.980392f, 0.960784f };
				cube->Material.Metalness = 1.0f;
				cube->Material.Roughness = 0.1f;

				objects.push_back(cube);
			}
		}
	}

	std::vector<std::shared_ptr<Light>> lights;
	{
		auto pLight = std::make_shared<Lights::Point>();
		pLight->Intensity = 28.0f;
		pLight->Colour = { 0.9f, 0.1f, 0.1f };
		pLight->ShadowIntensity = 1.0f;
		pLight->XForm.SetPosition({ 20.0f, 20.0f, 20.0f });

		lights.push_back(pLight);

		auto evLight = std::make_shared<Lights::Enviroment>(
			std::move(LoadImage("..\\..\\Assets\\EnviromentMaps\\Sky\\Top.png")),
			std::move(LoadImage("..\\..\\Assets\\EnviromentMaps\\Sky\\Bottom.png")),
			std::move(LoadImage("..\\..\\Assets\\EnviromentMaps\\Sky\\Left.png")),
			std::move(LoadImage("..\\..\\Assets\\EnviromentMaps\\Sky\\Right.png")),
			std::move(LoadImage("..\\..\\Assets\\EnviromentMaps\\Sky\\Back.png")),
			std::move(LoadImage("..\\..\\Assets\\EnviromentMaps\\Sky\\Front.png")));
		evLight->Intensity = 6.0f;

		lights.push_back(evLight);
	}

	auto camera = Camera(512.0f, 512.0f, 4.0f, 0.01f);
	camera.XForm.SetPosition({ -28.0f, 15.0f, -28.0f });
	camera.LookAt({ -5.0f, 0.0f, -5.0f }, Y_MINUS_AXIS);

	RayTracer::Settings settings;
	settings.SamplesPerPixel = 10u;
	settings.MaxDepth = 1u;
	settings.MaxGIDepth = 0u;
	settings.SecondryBounces = 0u;

	const auto RenderBlockCityScene = RayTracer(Scene(objects, lights, camera), settings).Render(&SaveImage, "Render_Update.png");
	SaveImage(RenderBlockCityScene, "Render_Cubes.png");
}