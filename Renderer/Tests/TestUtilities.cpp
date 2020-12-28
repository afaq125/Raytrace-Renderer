#include "Tests.h"

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

Texture LoadImage(const std::string& file, const bool normalise)
{
	Texture result;
	FREE_IMAGE_FORMAT format = FreeImage_GetFileType(file.c_str());

	if (format == FIF_UNKNOWN)
	{
		throw std::logic_error("Unable to open image file. Unknown type.");
	}

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
	kernel *= multiplier;
	return kernel;
}