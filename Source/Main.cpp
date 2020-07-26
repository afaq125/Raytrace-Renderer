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
			color.rgbRed = static_cast<BYTE>(image[0].Get(j, i) * 255.0);
			color.rgbGreen = static_cast<BYTE>(image[1].Get(j, i) * 255.0);
			color.rgbBlue = static_cast<BYTE>(image[2].Get(j, i) * 255.0);
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
		s1->Radius = 3.0;
		s2->Radius = 3.0;
		s3->Radius = 3.0;
		s1->XForm.SetPosition({ -6.0, 0.0, 0.0 });
		s2->XForm.SetPosition({ 0.0, 0.0, -3.0 });
		s3->XForm.SetPosition({ 6.0, 0.0, 0.0 });
		s1->Material.Colour = { 1.0, 0.0, 0.0 };
		s2->Material.Colour = { 0.0, 1.0, 0.0 };
		s3->Material.Colour = { 0.0, 0.0, 1.0 };
		//s2->Material.Reflective = true;
		objects.push_back(s1);
		objects.push_back(s2);
		objects.push_back(s3);

		auto p1 = std::make_shared<Sphere>();
		p1->Radius = 500.0;
		p1->XForm.SetPosition({ 0.0, -503.0, 0.0 });
		p1->Material.Colour = { 1.0, 1.0, 1.0 };
		objects.push_back(p1);
	}

	int w = 256;
	int h = 256;
	auto camera = Camera(w, h, 1.0, 0.01);
	camera.SetAspectRatio(16.0, 9.0);
	camera.XForm.SetPosition({0.0, 15.0, 20.0});
	
	Vector3 target = { 0.0, 0.0, 0.0 };
	camera.LookAt(target, Y_MINUS_AXIS);

	auto render = RayTracer(objects, camera).Render(&SaveImage, "Render_Update.png");
	//render[0].Transpose();
	//render[1].Transpose();
	//render[2].Transpose();
	//std::cout << render[0].Print() << std::endl;
	//std::cout << render[1].Print() << std::endl;
	//std::cout << render[2].Print() << std::endl;
	SaveImage(render, "Render_A.png");

	//auto image = LoadImage("../../../Common/Test_2.jpg");

	int input;
	std::cin >> input;

	return 0;
}