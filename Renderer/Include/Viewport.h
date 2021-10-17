#pragma once

namespace Renderer
{
    class Viewport
    {
    public:
        using Pixels = std::array<Matrix<float>, 3>;

        Viewport(const Size pixels_x, const Size pixels_y, const float pixel_spacing = 0.1f);

        void SetAspectRatio(const float a, const float b);
        void SetPixel(const Size index, const float r, const float g, const float b);

        Vector3 GetPixelValue(const Size index) const;
        Vector2 GetPixelUV(const Size index) const;
        Vector3 GetPixelPosition(const float u, const float v) const;
        const Pixels& GetPixels() const { return m_pixels; }
        Size Area() const { return m_pixels[0].Area(); }

    private:
        void Initialize();

        Size m_pixels_x;
        Size m_pixels_y;
        float m_pixel_spacing;
        Pixels m_pixels;
    };
}