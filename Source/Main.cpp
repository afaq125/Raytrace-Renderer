#pragma once

#include "FreeImage.h"
#include "Renderer.h"

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

Matrix<float> LoadImage(const std::string& file)
{
	auto result = Matrix<float>();
	FREE_IMAGE_FORMAT format = FreeImage_GetFileType(file.c_str());

	if (format != FIF_UNKNOWN)
	{
		FIBITMAP *bitmap = FreeImage_Load(format, file.c_str());
		unsigned int height = FreeImage_GetHeight(bitmap);
		unsigned int width = FreeImage_GetWidth(bitmap);
		unsigned int area = width * height;

		FIBITMAP *bitmapBlue = FreeImage_GetChannel(bitmap, FREE_IMAGE_COLOR_CHANNEL::FICC_BLUE);
		BYTE* pixelData = NULL;
		pixelData = FreeImage_GetBits(bitmapBlue);

		result = Matrix<float>(height, width);
		for (unsigned int j = 0; j < area; ++j)
		{
			result[j] = pixelData[j];
		}

		FreeImage_Unload(bitmap);
	}

	return result;
}

Matrix<float> GaussianKernel(const float multiplier)
{
	Matrix<float> kernel =
	{ { 0.003765f,	0.015019f,	0.023792f,	0.015019f,	0.003765f },
	{ 0.015019f,	0.059912f,	0.094907f,	0.059912f,	0.015019f },
	{ 0.023792f,	0.094907f,	0.150342f,	0.094907f,	0.023792f },
	{ 0.015019f,	0.059912f,	0.094907f,	0.059912f,	0.015019f },
	{ 0.003765f,	0.015019f,	0.023792f,	0.015019f,	0.003765f } };
	kernel *= 1.0f;
	return kernel;
}

int main()
{
	//Create scene.
	std::vector<std::shared_ptr<Object>> objects;
	{
		auto s1 = std::make_shared<Sphere>();
		auto s2 = std::make_shared<Sphere>();
		auto s3 = std::make_shared<Sphere>();
		s1->Radius = 3.3f;
		s2->Radius = 3.3f;
		s3->Radius = 3.3f;
		s1->XForm.SetPosition({ -6.0f, 3.0f, 0.0f });
		s2->XForm.SetPosition({ 0.0f, 3.0f, -3.0f });
		s3->XForm.SetPosition({ 6.0f, 3.0f, 0.0f });
		s1->Material.Colour = { 1.0f, 0.0f, 0.0f };
		s2->Material.Colour = { 1.0f, 1.0f, 1.0f };
		s3->Material.Colour = { 0.0f, 0.0f, 1.0f };
		s2->Material.Reflective = true;
		objects.push_back(s1);
		objects.push_back(s2);
		objects.push_back(s3);

		auto p1 = std::make_shared<Plane>();
		auto p2 = std::make_shared<Plane>();
		auto p3 = std::make_shared<Plane>();
		auto p4 = std::make_shared<Plane>();
		auto p5 = std::make_shared<Plane>();
		p1->Width = 2000.0f;
		p2->Width = 20.0f;
		p3->Width = 20.0f;
		p4->Width = 20.0f;
		p5->Width = 20.0f;
		p1->Height = 2000.0f;
		p2->Height = 20.0f;
		p3->Height = 20.0f;
		p4->Height = 20.0f;
		p5->Height = 20.0f;
		p1->XForm.SetPosition({ 0.0f, 0.0f, 0.0f });
		p2->XForm.SetPosition({ 0.0f, 20.0f, 0.0f });
		p3->XForm.SetPosition({ 10.0f, 10.0f, 0.0f });
		p4->XForm.SetPosition({ -10.0f, 10.0f, 0.0f });
		p5->XForm.SetPosition({ 0.0f, 10.0f, -10.0f });
		p1->Material.Colour = { 1.0f, 1.0f, 1.0f };
		p2->Material.Colour = { 1.0f, 1.0f, 1.0f };
		p3->Material.Colour = { 1.0f, 1.0f, 1.0f };
		p4->Material.Colour = { 1.0f, 1.0f, 1.0f };
		p5->Material.Colour = { 1.0f, 1.0f, 1.0f };
		p1->SetDirection({ 0.0f, 1.0f, 0.0f });
		p2->SetDirection({ 0.0f, -1.0f, 0.0f });
		p3->SetDirection({ -1.0f, 0.0f, 0.0f });
		p4->SetDirection({ 1.0f, 0.0f, 0.0f });
		p5->SetDirection({ 0.0f, 0.0f, -1.0f });
		objects.push_back(p1);
		//objects.push_back(p2);
		//objects.push_back(p3);
		//objects.push_back(p4);
		//objects.push_back(p5);
	}

	std::vector<std::shared_ptr<Light>> lights;
	{
		auto l1 = std::make_shared<Lights::Point>();
		auto l2 = std::make_shared<Lights::Area>();
		
		l1->Colour = { 1.0f, 0.0f, 0.0f };
		l1->ShadowIntensity = 1.0f;
		l1->XForm.SetPosition({ 0.0f, 10.0f, -10.0f });

		l2->Colour = { 1.0f, 1.0f, 0.0f };
		l2->Samples = 8u;
		l2->Grid->Width = 10.0f;
		l2->Grid->Height = 10.0f;
		//l2->RenderGeometry = true;
		l2->Grid->XForm.SetPosition({ 0.0f, 10.0f, -20.0f });
		l2->Grid->SetDirection({ 0.0f, -0.5f, -0.5f });

		//lights.push_back(l1);
		lights.push_back(l2);
	}

	const float w = 256.0f;
	const float h = 256.0f;
	const Vector3 target = { 0.0f, 3.0f, 0.0f };
	auto camera = Camera(w, h, 1.5f, 0.01f);
	camera.SetAspectRatio(16.0f, 9.0f);
	camera.XForm.SetPosition({0.0f, 5.0f, 20.0f});
	camera.LookAt(target, Y_MINUS_AXIS);

	const auto render = RayTracer(objects, lights, camera).Render(&SaveImage, "Render_Update.png");
	SaveImage(render, "Render_A.png");

	int input;
	std::cin >> input;

	return 0;
}