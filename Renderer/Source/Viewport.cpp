#include "Renderer.h"

using namespace Renderer;

Viewport::Viewport(const Size pixels_x, const Size pixels_y, const float pixel_spacing) :
    m_pixels_x(pixels_x),
    m_pixels_y(pixels_y),
    m_pixel_spacing(pixel_spacing)
{
    Initialize();
}

void Viewport::SetAspectRatio(const float a, const float b)
{
    const float ratio = a / b;
    m_pixels_x /= static_cast<Size>(ratio);
}

void Viewport::SetPixel(const Size index, const float r, const float g, const float b)
{
    m_pixels[0][index] = r;
    m_pixels[1][index] = g;
    m_pixels[2][index] = b;
}

Vector3 Viewport::GetPixelValue(const Size index) const
{
    return { m_pixels[0][index] , m_pixels[1][index] , m_pixels[2][index] };
}

Vector2 Viewport::GetPixelUV(const Size index) const
{
    const float column = static_cast<float>(m_pixels[0].ColumnNumberFromIndex(index));
    const float row = static_cast<float>(m_pixels[0].RowNumberFromIndex(index));
    const float u = column / static_cast<float>(m_pixels[0].Columns());
    const float v = row / static_cast<float>(m_pixels[0].Rows());
    return { u, v };
}

Vector3 Viewport::GetPixelPosition(const float u, const float v) const
{
    const float half_width = static_cast<float>(m_pixels_x) / 2.0f;
    const float half_height = static_cast<float>(m_pixels_y) / 2.0f;

    const float x = ((static_cast<float>(m_pixels_x) * u) - half_width) * m_pixel_spacing;
    const float y = ((static_cast<float>(m_pixels_y) * v) - half_height) * m_pixel_spacing;

    return { x, y, 0.0f };
}

void Viewport::Initialize()
{
    m_pixels[0] = Matrix<float>(0.0, m_pixels_y, m_pixels_x);
    m_pixels[1] = Matrix<float>(0.0, m_pixels_y, m_pixels_x);
    m_pixels[2] = Matrix<float>(0.0, m_pixels_y, m_pixels_x);
}