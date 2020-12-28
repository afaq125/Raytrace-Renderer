#include "Tests.h"

using namespace Renderer;
using namespace Renderer::Math;

TEST_F(RendererUnitTests, GlobalIlluminationTest)
{
	Scene scene;
	const float width = 512.0f;
	const float height = 512.0f;
	const Vector3 target = { 0.0f, 3.0f, 0.0f };
	auto camera = Camera(width, height, 1.5f, 0.01f);
	camera.SetAspectRatio(16.0f, 9.0f);
	camera.XForm.SetPosition({ 0.0f, 5.0f, 20.0f });
	camera.LookAt(target, Y_MINUS_AXIS);

	auto sphere1 = std::make_shared<Sphere>();
	auto sphere2 = std::make_shared<Sphere>();
	auto sphere3 = std::make_shared<Sphere>();
	sphere1->Radius = 3.3f;
	sphere2->Radius = 3.3f;
	sphere3->Radius = 3.3f;
	sphere1->XForm.SetPosition({ -6.0f, 3.0f, 0.0f });
	sphere2->XForm.SetPosition({ 0.0f, 3.0f, -3.0f });
	sphere3->XForm.SetPosition({ 6.0f, 3.0f, 0.0f });
	sphere1->Material.Albedo = { 1.0f, 0.0f, 0.0f };
	sphere2->Material.Albedo = { 1.0f, 1.0f, 1.0f };
	sphere3->Material.Albedo = { 0.0f, 1.0f, 1.0f };
	sphere1->Material.Metalness = 1.0f;
	sphere2->Material.Metalness = 1.0f;
	sphere3->Material.Metalness = 1.0f;
	sphere1->Material.Roughness = 0.1f;
	sphere2->Material.Roughness = 0.2f;
	sphere3->Material.Roughness = 0.1f;

	auto plane1 = std::make_shared<Plane>();
	plane1->Width = 2000.0f;
	plane1->Height = 2000.0f;
	plane1->XForm.SetPosition({ 0.0f, 0.0f, 0.0f });
	plane1->SetDirection({ 0.0f, 1.0f, 0.0f });
	plane1->Material.Albedo = { 1.0f, 1.0f, 1.0f };
	plane1->Material.Metalness = 0.0f;
	plane1->Material.Roughness = 1.0f;
	plane1->Material.ReflectionSamples = 0u;
	plane1->Material.ReflectionDepth = 0u;

	auto pointLight1 = std::make_shared<Lights::Point>();
	pointLight1->Intensity = 18.0f;
	pointLight1->Colour = { 0.9f, 0.9f, 0.9f };
	pointLight1->ShadowIntensity = 1.0f;
	pointLight1->XForm.SetPosition({ 10.0f, 10.0f, 10.0f });

	auto enviromentLight = std::make_shared<Lights::Enviroment>(
		std::move(LoadImage("..\\Assets\\EnviromentMaps\\Garage\\Top.png")),
		std::move(LoadImage("..\\Assets\\EnviromentMaps\\Garage\\Bottom.png")),
		std::move(LoadImage("..\\Assets\\EnviromentMaps\\Garage\\Left.png")),
		std::move(LoadImage("..\\Assets\\EnviromentMaps\\Garage\\Right.png")),
		std::move(LoadImage("..\\Assets\\EnviromentMaps\\Garage\\Back.png")),
		std::move(LoadImage("..\\Assets\\EnviromentMaps\\Garage\\Front.png")));
	enviromentLight->Intensity = 10.0f;

	scene.Cam = camera;

	scene.Objects.push_back(sphere1);
	scene.Objects.push_back(sphere2);
	scene.Objects.push_back(sphere3);
	scene.Objects.push_back(plane1);

	scene.Lights.push_back(pointLight1);
	scene.Lights.push_back(enviromentLight);

	const auto RenderGIScene = RayTracer(scene).Render(&SaveImage, "Render_Update.png");
	SaveImage(RenderGIScene, "Render_GI.png");
}

TEST_F(RendererUnitTests, PBRTest)
{
	Scene scene;
	const float width = 512.0f;
	const float height = 512.0f;
	const Vector3 target = { 0.0f, 3.0f, 0.0f };
	auto camera = Camera(width, height, 1.5f, 0.01f);
	camera.SetAspectRatio(16.0f, 9.0f);
	camera.XForm.SetPosition({ 0.0f, 5.0f, 20.0f });
	camera.LookAt(target, Y_MINUS_AXIS);

	Size rows = 5;
	Size columns = 5;
	Size area = rows * columns;
	float spacing = 4.0f;
	for (Size i = 0; i < area; ++i)
	{
		Size c = i / columns;
		Size r = i % rows;
		float x = (static_cast<float>(c) * spacing) - static_cast<float>(rows) - spacing;
		float y = (static_cast<float>(r) * spacing) - static_cast<float>(columns) - spacing;
		float z = 0.0f;
		Vector3 position = { x, y, z };

		auto sphere = std::make_shared<Sphere>();
		sphere->Radius = 1.75f;
		sphere->XForm.SetPosition(position);
		sphere->Material.Albedo = { 1.0f, 1.0f, 1.0f };
		sphere->Material.Metalness = Clamp((1.0f / rows) * r, 0.0f, 1.0f);
		sphere->Material.Roughness = Clamp((1.0f / columns) * c, 0.05f, 1.0f);
		sphere->Material.Albedo[0] = sphere->Material.Metalness;
		scene.Objects.push_back(sphere);
	}

	float pointLightIntensity = 18.0f;
	Vector3 pointLightColour = { 0.9f, 0.9f, 0.9f };
	float pointLightDistance = 10.0f;

	auto pointLight1 = std::make_shared<Lights::Point>();
	pointLight1->Intensity = pointLightIntensity;
	pointLight1->Colour = pointLightColour;
	pointLight1->ShadowIntensity = 1.0f;
	pointLight1->XForm.SetPosition({ pointLightDistance, pointLightDistance, pointLightDistance });

	auto pointLight2 = std::make_shared<Lights::Point>();
	pointLight2->Intensity = pointLightIntensity;
	pointLight2->Colour = pointLightColour;
	pointLight2->ShadowIntensity = 1.0f;
	pointLight2->XForm.SetPosition({ -pointLightDistance, pointLightDistance, pointLightDistance });

	auto pointLight3 = std::make_shared<Lights::Point>();
	pointLight3->Intensity = pointLightIntensity;
	pointLight3->Colour = pointLightColour;
	pointLight3->ShadowIntensity = 1.0f;
	pointLight3->XForm.SetPosition({ pointLightDistance, -pointLightDistance, pointLightDistance });

	auto pointLight4 = std::make_shared<Lights::Point>();
	pointLight4->Intensity = pointLightIntensity;
	pointLight4->Colour = pointLightColour;
	pointLight4->ShadowIntensity = 1.0f;
	pointLight4->XForm.SetPosition({ -pointLightDistance, -pointLightDistance, pointLightDistance });

	auto enviromentLight = std::make_shared<Lights::Enviroment>(
		std::move(LoadImage("..\\Assets\\EnviromentMaps\\Garage\\Top.png")),
		std::move(LoadImage("..\\Assets\\EnviromentMaps\\Garage\\Bottom.png")),
		std::move(LoadImage("..\\Assets\\EnviromentMaps\\Garage\\Left.png")),
		std::move(LoadImage("..\\Assets\\EnviromentMaps\\Garage\\Right.png")),
		std::move(LoadImage("..\\Assets\\EnviromentMaps\\Garage\\Back.png")),
		std::move(LoadImage("..\\Assets\\EnviromentMaps\\Garage\\Front.png")));
	enviromentLight->Intensity = 10.0f;

	scene.Cam = camera;

	scene.Lights.push_back(pointLight1);
	scene.Lights.push_back(pointLight2);
	scene.Lights.push_back(pointLight3);
	scene.Lights.push_back(pointLight4);
	scene.Lights.push_back(enviromentLight);

	const auto RenderPBRScene = RayTracer(scene).Render(&SaveImage, "Render_Update.png");
	SaveImage(RenderPBRScene, "Render_PBR.png");
}

TEST_F(RendererUnitTests, BlocksTest)
{
	Scene scene;
	const float width = 512.0f;
	const float height = 512.0f;
	const Vector3 target = { -5.0f, 0.0f, -5.0f };
	auto camera = Camera(width, height, 4.0f, 0.01f);
	//camera.SetAspectRatio(16.0f, 9.0f);
	camera.XForm.SetPosition({ -28.0f, 15.0f, -28.0f });
	camera.LookAt(target, Y_MINUS_AXIS);

	auto plane1 = std::make_shared<Plane>();
	plane1->Width = 10.0f;
	plane1->Height = 10.0f;
	plane1->XForm.SetPosition({ 0.0f, 0.0f, 0.0f });
	plane1->SetDirection({ 0.0f, 1.0f, 0.0f });
	plane1->Material.Albedo = { 1.0f, 1.0f, 1.0f };
	plane1->Material.Metalness = 0.0f;
	plane1->Material.Roughness = 1.0f;

	auto map = LoadImage("..\\Assets\\Height_Map.png").Pixels[0];
	map *= 8.0f;
	map += 1.0f;
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
			cube->Material.Roughness = 0.6f;
			cube->Material.ReflectionDepth = 0u;
			cube->Material.ReflectionSamples = 0u;

			scene.Objects.push_back(cube);
		}
	}

	auto pointLight1 = std::make_shared<Lights::Point>();
	pointLight1->Intensity = 28.0f;
	pointLight1->Colour = { 0.9f, 0.1f, 0.1f };
	pointLight1->ShadowIntensity = 1.0f;
	pointLight1->XForm.SetPosition({ 20.0f, 20.0f, 20.0f });

	auto areaLight1 = std::make_shared<Lights::Area>();
	areaLight1->Intensity = 18.0f;
	areaLight1->Colour = { 0.9f, 0.1f, 0.1f };
	areaLight1->ShadowIntensity = 1.0f;
	areaLight1->Grid->Width = 10.0f;
	areaLight1->Grid->Height = 10.0f;
	areaLight1->Grid->XForm.SetPosition({ 0.0f, 0.0f, -50.0f });
	areaLight1->Grid->SetDirection({ 0.0f, 0.0f, 1.0f });

	auto enviromentLight = std::make_shared<Lights::Enviroment>(
		std::move(LoadImage("..\\Assets\\EnviromentMaps\\Garage\\Top.png")),
		std::move(LoadImage("..\\Assets\\EnviromentMaps\\Garage\\Bottom.png")),
		std::move(LoadImage("..\\Assets\\EnviromentMaps\\Garage\\Left.png")),
		std::move(LoadImage("..\\Assets\\EnviromentMaps\\Garage\\Right.png")),
		std::move(LoadImage("..\\Assets\\EnviromentMaps\\Garage\\Back.png")),
		std::move(LoadImage("..\\Assets\\EnviromentMaps\\Garage\\Front.png")));
	enviromentLight->Intensity = 2.0f;

	scene.Cam = camera;

	//scene.Objects.push_back(plane1);

	scene.Lights.push_back(pointLight1);
	scene.Lights.push_back(enviromentLight);

	const auto RenderBlockCityScene = RayTracer(scene).Render(&SaveImage, "Render_Update.png");
	SaveImage(RenderBlockCityScene, "Render_BlockCity.png");
}


TEST_F(RendererUnitTests, SpheresTest)
{
	Scene scene;
	const float width = 512.0f;
	const float height = 512.0f;
	const Vector3 target = { 0.0f, 0.0f, 0.0f };
	auto camera = Camera(width, height, 1.5f, 0.01f);
	//camera.SetAspectRatio(16.0f, 9.0f);
	camera.XForm.SetPosition({ 0.0f, 5.0f, 20.0f });
	camera.LookAt(target, Y_MINUS_AXIS);

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
				sphere->Material.ReflectionDepth = values[9];
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
		constexpr Size limit = 1000;
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
			const auto hemisphereSample = SampleHemisphere(Random(), Random()) * invert;
			
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
			sphere->Material.Roughness = Random() * 0.25f;
			sphere->Material.ReflectionDepth = 1u;
			spheres.push_back(sphere);

			++i;
		}

		auto sphere = std::make_shared<Sphere>();
		sphere->Radius = scale;
		sphere->XForm.SetPosition(Vector3());
		sphere->Material.Albedo = { 0.5f, 0.5f, 0.5f };
		sphere->Material.Metalness = 0.0f;
		sphere->Material.Roughness = 0.05f;
		sphere->Material.ReflectionDepth = 10u;
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

	auto pointLight1 = std::make_shared<Lights::Point>();
	pointLight1->Intensity = 28.0f;
	pointLight1->Colour = { 0.9f, 0.1f, 0.1f };
	pointLight1->ShadowIntensity = 1.0f;
	pointLight1->XForm.SetPosition({ 20.0f, 20.0f, 20.0f });

	auto areaLight1 = std::make_shared<Lights::Area>();
	areaLight1->Intensity = 18.0f;
	areaLight1->Colour = { 0.9f, 0.1f, 0.1f };
	areaLight1->ShadowIntensity = 1.0f;
	areaLight1->Grid->Width = 10.0f;
	areaLight1->Grid->Height = 10.0f;
	areaLight1->Grid->XForm.SetPosition({ 0.0f, 0.0f, -50.0f });
	areaLight1->Grid->SetDirection({ 0.0f, 0.0f, 1.0f });

	auto enviromentLight = std::make_shared<Lights::Enviroment>(
		std::move(LoadImage("..\\..\\Assets\\EnviromentMaps\\Garage\\Top.png")),
		std::move(LoadImage("..\\..\\Assets\\EnviromentMaps\\Garage\\Bottom.png")),
		std::move(LoadImage("..\\..\\Assets\\EnviromentMaps\\Garage\\Left.png")),
		std::move(LoadImage("..\\..\\Assets\\EnviromentMaps\\Garage\\Right.png")),
		std::move(LoadImage("..\\..\\Assets\\EnviromentMaps\\Garage\\Back.png")),
		std::move(LoadImage("..\\..\\Assets\\EnviromentMaps\\Garage\\Front.png")));
	enviromentLight->Intensity = 10.0f;

	scene.Cam = camera;

	//scene.Lights.push_back(pointLight1);
	//scene.Lights.push_back(areaLight1);
	scene.Lights.push_back(enviromentLight);

	RayTracer::Settings settings;
	settings.MaxDepth = 2u;
	settings.MaxGIDepth = 2u;
	settings.SamplesPerPixel = 10u;
	settings.SecondryBounces = 1u;

	const auto RenderPBRScene = RayTracer(scene, settings).Render(&SaveImage, "Render_Update.png");
	SaveImage(RenderPBRScene, "Render_Spheres.png");
}