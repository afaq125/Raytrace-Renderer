#pragma once

void SaveImage(const std::array<Renderer::Math::Matrix<float>, 3>& image, const std::string& path);
Renderer::Texture LoadImage(const std::string& file, const bool normalise = true);

Renderer::Math::Matrix<float> GaussianKernel(const float multiplier = 1.0f);