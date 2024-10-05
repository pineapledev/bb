#include "texture.h"

#ifdef NIT_GRAPHICS_API_OPENGL

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

namespace Nit
{
    void SetMagFilter(const u32 texture_id, const MagFilter mag_filter)
    {
        switch (mag_filter)
        {
        case MagFilter::Linear:
            glTextureParameteri(texture_id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            return;
        case MagFilter::Nearest:
            glTextureParameteri(texture_id, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }
    }

    void SetMinFilter(const u32 texture_id, const MinFilter mag_filter)
    {
        switch (mag_filter)
        {
        case MinFilter::Linear:
            glTextureParameteri(texture_id, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            return;
        case MinFilter::Nearest:
            glTextureParameteri(texture_id, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        }
    }

    void SetWrapMode(const u32 texture_id, const TextureCoordinate texture_coordinate, const WrapMode wrap_mode)
    {
        const GLenum coord = texture_coordinate == TextureCoordinate::U ? GL_TEXTURE_WRAP_S : GL_TEXTURE_WRAP_T;

        switch (wrap_mode)
        {
        case WrapMode::Repeat:
            glTextureParameteri(texture_id, coord, GL_REPEAT);
            return;
        case WrapMode::ClampToEdge:
            glTextureParameteri(texture_id, coord, GL_CLAMP_TO_EDGE);
        }
    }

    void RegisterTexture2D()
    {
    }

    void FreeTextureImage(Texture2D* texture)
    {
        if (!texture->pixel_data || !texture->loaded_from_image)
        {
            return;
        }
        stbi_image_free(texture->pixel_data);
        texture->pixel_data = nullptr;
    }

    void LoadTexture2D(Texture2D* texture)
    {
        if (texture->loaded)
        {
            FreeTexture2D(texture);
        }

        static constexpr u32 WHITE_TEXTURE_DATA = 0xffffffff;

        if (texture->is_white_texture)
        {
            texture->width    = 1;
            texture->height   = 1;
            texture->channels = 4;
        }
        else if (!texture->image_path.empty())
        {
            stbi_set_flip_vertically_on_load(1);
            i32 width, height, channels;

            texture->pixel_data = stbi_load(texture->image_path.c_str(), &width, &height, &channels, 0);

            texture->width    = static_cast<u32>(width);
            texture->height   = static_cast<u32>(height);
            texture->channels = static_cast<u32>(channels);

            texture->loaded_from_image = true;
        }

        GLenum internal_format = 0, data_format = 0;
        
        if (texture->channels == 4)
        {
            internal_format = GL_RGBA8;
            data_format = GL_RGBA;
        }
        else if (texture->channels == 3)
        {
            internal_format = GL_RGB8;
            data_format = GL_RGB;
        }

        void* data_to_upload = nullptr;
        data_to_upload = !texture->is_white_texture ? (void*) texture->pixel_data : (void*) &WHITE_TEXTURE_DATA;

        if (!data_to_upload)
        {
            NIT_CHECK_MSG(false, "Trying to load empty texture!");
            return;
        }
        
        glCreateTextures(GL_TEXTURE_2D, 1, &texture->id);
        glTextureStorage2D(texture->id, 1, internal_format, texture->width, texture->height);

        SetMinFilter(texture->id, texture->min_filter);
        SetMagFilter(texture->id, texture->mag_filter);

        SetWrapMode(texture->id, TextureCoordinate::U, texture->wrap_mode_u);
        SetWrapMode(texture->id, TextureCoordinate::V, texture->wrap_mode_v);
        
        texture->size = {static_cast<f32>(texture->width), static_cast<f32>(texture->height)};
        
        glTextureSubImage2D(texture->id, 0, 0, 0, texture->width, texture->height, data_format,
            GL_UNSIGNED_BYTE, data_to_upload);
    
        texture->loaded = true;
        
        if (!texture->keep_pixel_data)
        {
            texture->pixel_data = nullptr;
            
            if (texture->loaded_from_image)
            {
                FreeTextureImage(texture);
            }
        }
    }

    void FreeTexture2D(Texture2D* texture)
    {
        if (!texture->loaded)
        {
            return;
        }
        
        glDeleteTextures(1, &texture->id);
        texture->loaded = false;

        FreeTextureImage(texture);
    }

    void BindTexture2D(const Texture2D& texture, u32 slot)
    {
        glBindTextureUnit(slot, texture.id);
    }
}

#endif