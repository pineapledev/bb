#include "texture.h"

#ifdef NIT_GRAPHICS_API_OPENGL
#include <glad/glad.h>
#endif

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace Nit
{
    Image::Image() = default;

    Image::Image(const char* file_path)
    {
        Load(file_path);
    }

    Image::Image(Image&& other) noexcept
        : data(other.data)
          , width(other.width)
          , height(other.height)
          , channels(other.channels)
    {
        other.data = nullptr;
    }

    Image::~Image()
    {
        Free();
    }

    Image& Image::operator=(Image&& other) noexcept
    {
        data = other.data;
        width = other.width;
        height = other.height;
        channels = other.channels;

        other.data = nullptr;
        return *this;
    }

    void Image::Load(const char* file_path)
    {
        if (data)
        {
            Free();
        }

        stbi_set_flip_vertically_on_load(1);
        i32 w, h, c;
        data = stbi_load(file_path, &w, &h, &c, 0);

        if (!data)
        {
            NIT_CHECK_MSG(false, "Image not found!");
            return;
        }

        width = static_cast<u32>(w);
        height = static_cast<u32>(h);
        channels = static_cast<u32>(c);
    }

    void Image::Free()
    {
        if (!data)
        {
            return;
        }
        stbi_image_free(data);
        data = nullptr;
    }

    void SetMagFilter(const u32 texture_id, const MagFilter mag_filter)
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        switch (mag_filter)
        {
        case MagFilter::Linear:
            glTextureParameteri(texture_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            return;
        case MagFilter::Nearest:
            glTextureParameteri(texture_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }
#endif
    }

    void SetMinFilter(const u32 texture_id, const MinFilter mag_filter)
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        switch (mag_filter)
        {
        case MinFilter::Linear:
            glTextureParameteri(texture_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            return;
        case MinFilter::Nearest:
            glTextureParameteri(texture_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        }
#endif
    }

    void SetWrapMode(const u32 texture_id, const TextureCoordinate texture_coordinate, const WrapMode wrap_mode)
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        const GLenum coord = texture_coordinate == TextureCoordinate::U ? GL_TEXTURE_WRAP_S : GL_TEXTURE_WRAP_T;

        switch (wrap_mode)
        {
        case WrapMode::Repeat:
            glTextureParameteri(texture_id, coord, GL_REPEAT);
            return;
        case WrapMode::ClampToEdge:
            glTextureParameteri(texture_id, coord, GL_CLAMP_TO_EDGE);
        }
#endif
    }

    Texture2D::Texture2D() = default;

    Texture2D::Texture2D(const void* data, u32 width, u32 height, u32 channels, const Texture2DCfg& cfg /*= {}*/)
    {
        UploadToGPU(data, width, height, channels, cfg);
    }

    Texture2D::Texture2D(const Image& image, const Texture2DCfg& cfg /*= {}*/)
    {
        UploadToGPU(image, cfg);
    }

    Texture2D::~Texture2D()
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        glDeleteTextures(1, &id);
#endif
    }

    void Texture2D::UploadToGPU(const void* data, u32 width, u32 height, u32 channels, const Texture2DCfg& cfg)
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        NIT_CHECK_MSG(data, "Missing texture data!");
        NIT_CHECK_MSG(width && height, "Invalid width or height!");

        if (uploaded)
        {
            glDeleteTextures(1, &id);
            uploaded = false;
        }

        GLenum internal_format = 0, data_format = 0;

        if (channels == 4)
        {
            internal_format = GL_RGBA8;
            data_format = GL_RGBA;
        }
        else if (channels == 3)
        {
            internal_format = GL_RGB8;
            data_format = GL_RGB;
        }

        glCreateTextures(GL_TEXTURE_2D, 1, &id);
        glTextureStorage2D(id, 1, internal_format, width, height);

        SetMinFilter(id, cfg.min_filter);
        SetMagFilter(id, cfg.mag_filter);

        SetWrapMode(id, TextureCoordinate::U, cfg.wrap_mode_u);
        SetWrapMode(id, TextureCoordinate::V, cfg.wrap_mode_v);

        size = {static_cast<f32>(width), static_cast<f32>(height)};
        glTextureSubImage2D(id, 0, 0, 0, width, height,
                            data_format, GL_UNSIGNED_BYTE, data);

        uploaded = true;
#endif
    }

    void Texture2D::UploadToGPU(const Image& image, const Texture2DCfg& cfg)
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        if (!image.data)
        {
            NIT_CHECK_MSG(false, "Image is not loaded!");
            return;
        }

        UploadToGPU(image.data, image.width, image.height, image.channels, cfg);
#endif
    }

    void Texture2D::Bind(const u32 slot) const
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        glBindTextureUnit(slot, id);
#endif
    }
}