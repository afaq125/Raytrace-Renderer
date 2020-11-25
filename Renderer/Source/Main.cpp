#pragma once

#include "FreeImage.h"
#include "Renderer.h"
#include <cctype>

using namespace Renderer;
using namespace Renderer::Math;

void SaveImage(const std::array<Matrix<float>, 3>& image, const std::string& path)
{
	const int w = static_cast<int>(image[0].Columns());
	const int h = static_cast<int>(image[0].Rows());
	FIBITMAP *bitmapOutput = FreeImage_Allocate(w, h, 24, 0, 0, 0);
	RGBQUAD color;

	for (int i = 0; i < w; i++)
	{
		for (int j = 0; j < h; j++)
		{
			color.rgbRed = static_cast<BYTE>(std::round(image[0].Get(j, i) * 255.0));
			color.rgbGreen = static_cast<BYTE>(std::round(image[1].Get(j, i) * 255.0));
			color.rgbBlue = static_cast<BYTE>(std::round(image[2].Get(j, i) * 255.0));
			FreeImage_SetPixelColor(bitmapOutput, i, j, &color);
		}
	}

	FREE_IMAGE_FORMAT outputFormat = FreeImage_GetFIFFromFilename(path.c_str());

	if (outputFormat != FIF_UNKNOWN) 
	{
		FreeImage_Save(outputFormat, bitmapOutput, path.c_str(), 0);
	}
}

Texture LoadImage(const std::string& file, const bool normalise = true)
{
	Texture result;
	FREE_IMAGE_FORMAT format = FreeImage_GetFileType(file.c_str());

	if (format != FIF_UNKNOWN)
	{
		FIBITMAP *bitmap = FreeImage_Load(format, file.c_str());
		const unsigned int height = FreeImage_GetHeight(bitmap);
		const unsigned int width = FreeImage_GetWidth(bitmap);
		result = Texture(width, height);
		RGBQUAD color;

		for (unsigned int i = 0; i < width; i++)
		{
			for (unsigned int j = 0; j < height; j++)
			{
				FreeImage_GetPixelColor(bitmap, i, j, &color);
				float r = static_cast<float>(color.rgbRed);
				float g = static_cast<float>(color.rgbGreen);
				float b = static_cast<float>(color.rgbBlue);
				if (normalise)
				{
					r /= 255.0f;
					g /= 255.0f;
					b /= 255.0f;
				}
				result.Pixels[0].Set(i, j, r);
				result.Pixels[1].Set(i, j, g);
				result.Pixels[2].Set(i, j, b);
			}
		}

		FreeImage_Unload(bitmap);
	}

	return result;
}

Matrix<float> GaussianKernel(const float multiplier = 1.0f)
{
	Matrix<float> kernel =
	{ { 0.003765f,	0.015019f,	0.023792f,	0.015019f,	0.003765f },
	{ 0.015019f,	0.059912f,	0.094907f,	0.059912f,	0.015019f },
	{ 0.023792f,	0.094907f,	0.150342f,	0.094907f,	0.023792f },
	{ 0.015019f,	0.059912f,	0.094907f,	0.059912f,	0.015019f },
	{ 0.003765f,	0.015019f,	0.023792f,	0.015019f,	0.003765f } };
	kernel *= multiplier;
	return kernel;
}

int main()
{
	Scene giScene;
	{
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

		giScene.Cam = camera;

		giScene.Objects.push_back(sphere1);
		giScene.Objects.push_back(sphere2);
		giScene.Objects.push_back(sphere3);
		giScene.Objects.push_back(plane1);

		giScene.Lights.push_back(pointLight1);
		giScene.Lights.push_back(enviromentLight);
	}

	Scene pbrScene;
	{
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
			pbrScene.Objects.push_back(sphere);
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

		pbrScene.Cam = camera;

		pbrScene.Lights.push_back(pointLight1);
		pbrScene.Lights.push_back(pointLight2);
		pbrScene.Lights.push_back(pointLight3);
		pbrScene.Lights.push_back(pointLight4);
		pbrScene.Lights.push_back(enviromentLight);
	}

	Scene blockCityScene;
	{
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
				const float y =  map.Get(c, r) / 2.0f;
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

				blockCityScene.Objects.push_back(cube);
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

		blockCityScene.Cam = camera;

		//blockCityScene.Objects.push_back(plane1);

		blockCityScene.Lights.push_back(pointLight1);
		blockCityScene.Lights.push_back(enviromentLight);
	}

	//const auto RenderGIScene = RayTracer(giScene).Render(&SaveImage, "Render_Update.png");
	//SaveImage(RenderGIScene, "Render_GI.png");

	//const auto RenderPBRScene = RayTracer(pbrScene).Render(&SaveImage, "Render_Update.png");
	//SaveImage(RenderPBRScene, "Render_PBR.png");

	const auto RenderBlockCityScene = RayTracer(blockCityScene).Render(&SaveImage, "Render_Update.png");
	SaveImage(RenderBlockCityScene, "Render_BlockCity.png");

	int input;
	std::cin >> input;

	return 0;
}