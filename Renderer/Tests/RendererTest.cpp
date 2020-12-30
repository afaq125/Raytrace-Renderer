#include "Tests.h"

using namespace Renderer;
using namespace Renderer::Math;

TEST_F(RendererUnitTests, GlobalIlluminationTest)
{
	Scene scene;

	std::vector<Vector3> planePositions = {
		{ 5.0f, 0.0f, 0.0f },
		{ 0.0f, 5.0f, 0.0f },
		{ -5.0f, 0.0f, 0.0f },
		{ 0.0f, -5.0f, 0.0f }
	};

	std::vector<Vector3> planeDirections = {
		{ -1.0f, 0.0f, 0.0f },
		{ 0.0f, -1.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f },
		{ 0.0f, 1.0f, 0.0f }
	};

	std::vector<Vector3> colours = {
		{ 68.0f, 75.0f, 91.0f },
		{ 205.0f, 90.0f, 87.0f },
		{ 120.0f, 164.0f, 163.0f },
		{ 225.0f, 177.0f, 105.0f },
		{ 225.0f, 177.0f, 105.0f },
		{ 170.0f, 170.0f, 170.0f }
	};

	for (auto& colour : colours)
	{
		colour = colour / 255.0f;
		colour = colour * 1.25f;
	}

	for (Size i = 0; i < planePositions.size(); ++i)
	{
		auto plane = std::make_shared<Plane>();
		plane->Width = 10.0f;
		plane->Height = 10.0f;
		plane->XForm.SetPosition(planePositions[i]);
		plane->SetDirection(planeDirections[i]);
		plane->Material.Albedo = { 1.0f, 1.0f, 1.0f };
		plane->Material.Metalness = 0.0f;
		plane->Material.Roughness = 1.0f;
		plane->Material.ReflectionSamples = 0u;
		plane->Material.ReflectionDepth = 0u;
		scene.Objects.push_back(plane);
	}

	constexpr Size count = 10;
	for (Size i = 0; i < planePositions.size(); ++i)
	{
		auto plane = std::static_pointer_cast<Plane>(scene.Objects[i]);
		for (Size j = 0; j < count; ++j)
		{
			const int randomIndex = static_cast<int>(Random() * static_cast<float>(colours.size()));
			const auto position = plane->UVToWorld(Random(), Random());
			const float radius = Random() * 2.0f;
			const Vector3 colour = colours[randomIndex];

			auto sphere = std::make_shared<Sphere>();
			sphere->Radius = radius;
			sphere->XForm.SetPosition(position);
			sphere->Material.Albedo = colour;
			sphere->Material.Metalness = 1.0f;
			sphere->Material.Roughness = 0.5f;
			scene.Objects.push_back(sphere);
		}
	}

	for (Size i = 0; i < planePositions.size(); ++i)
	{
		auto plane = std::static_pointer_cast<Plane>(scene.Objects[i]);
		plane->Width = 20.0f;
		plane->Height = 20.0f;
	}

	auto areaLight1 = std::make_shared<Lights::Area>();
	//areaLight1->RenderGeometry = true;
	areaLight1->Intensity = 28.0f;
	areaLight1->Samples = 32;
	areaLight1->Colour = { 1.0f, 1.0f, 1.0f };
	areaLight1->ShadowIntensity = 0.5f;
	areaLight1->Grid->Width = 10.0f;
	areaLight1->Grid->Height = 10.0f;
	areaLight1->Grid->XForm.SetPosition({ 0.0f, 0.0f, -9.9f });
	areaLight1->Grid->SetDirection({ 0.0f, 0.0f, 1.0f });

	const float width = 1024.0f;
	const float height = 1024.0f;
	const Vector3 target = { 0.0f, 0.0f, 0.0f };
	auto camera = Camera(width, height, 1.5f, 0.005f);
	camera.XForm.SetPosition({ 0.0f, 0.0f, 9.0f });
	camera.LookAt(target, Y_MINUS_AXIS);

	scene.Cam = camera;

	scene.Lights.push_back(areaLight1);

	RayTracer::Settings settings;
	settings.SamplesPerPixel = 20u;
	settings.MaxDepth = 2u;
	settings.MaxGIDepth = 1u;
	settings.SecondryBounces = 10u;

	scene.Initialise();
	const auto RenderGIScene = RayTracer(scene, settings).Render(&SaveImage, "Render_Update.png");
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

		for (auto& colour : colours)
		{
			colour = colour / 255.0f;
			colour = colour * 1.25f;
		}

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

			int randomIndex = static_cast<int>(Random() * static_cast<float>(colours.size()));

			auto sphere = std::make_shared<Sphere>();
			sphere->Radius = radius;
			sphere->XForm.SetPosition(position);
			sphere->Material.Albedo = colours[randomIndex];
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

	const std::string file = "./spheres.txt";
	scene.Objects = ReadFile(file);
	if (scene.Objects.empty()) 
	{
		scene.Objects = CreateSpheres();

		for (const auto object : scene.Objects)
		{
			const auto sphere = std::static_pointer_cast<Sphere>(object);
			WriteFile(file, *sphere.get());
		}
	}

	auto enviromentLight = std::make_shared<Lights::Enviroment>(
		std::move(LoadImage("..\\..\\Assets\\EnviromentMaps\\Sky\\Top.png")),
		std::move(LoadImage("..\\..\\Assets\\EnviromentMaps\\Sky\\Bottom.png")),
		std::move(LoadImage("..\\..\\Assets\\EnviromentMaps\\Sky\\Left.png")),
		std::move(LoadImage("..\\..\\Assets\\EnviromentMaps\\Sky\\Right.png")),
		std::move(LoadImage("..\\..\\Assets\\EnviromentMaps\\Sky\\Back.png")),
		std::move(LoadImage("..\\..\\Assets\\EnviromentMaps\\Sky\\Front.png")));
	enviromentLight->Intensity = 10.0f;
	
	scene.Lights.push_back(enviromentLight);

	const float width = 1024.0f;
	const float height = 1024.0f;
	const Vector3 target = { 0.0f, 0.0f, 0.0f };
	auto camera = Camera(width, height, 1.5f, 0.005f);
	camera.XForm.SetPosition({ 0.0f, 15.0f, 0.0f });
	camera.LookAt(target, Z_MINUS_AXIS);
	scene.Cam = camera;

	RayTracer::Settings settings;
	settings.SamplesPerPixel = 40u;
	settings.MaxDepth = 1u;
	settings.MaxGIDepth = 1u;
	settings.SecondryBounces = 4u;

	const auto RenderPBRScene = RayTracer(scene, settings).Render(&SaveImage, "Render_Update.png");
	SaveImage(RenderPBRScene, "Render_Spheres.png");
}
