#pragma once

#include <memory>
#include <stb_image.h>

#include "Texture.h"

struct StbImage
{
    stbi_uc *pixels;
    VkExtent3D extent;
    VkFormat format;
    std::string filepath;
};

class Texture2D : public Texture
{
public:
	~Texture2D() override;
	explicit Texture2D(StbImage Image);
    static std::unique_ptr<Texture2D> LoadTexture(std::string_view Filepath);

private:
    Texture2D() = delete;

public:
    std::string filepath;
};