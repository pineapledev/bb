#include "texture.h"

#ifdef NIT_GRAPHICS_API_OPENGL

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include "nit/core/asset.h"

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
    
    void InitSubTexture2DCoordinates(
          Vector2&       top_right
        , Vector2&       bottom_left
        , const Vector2& texture_size
        , const Vector2& sub_texture_size
        , const Vector2& location_in_atlas
    )
    {
        top_right.x = (location_in_atlas.x + sub_texture_size.x) * (1 / texture_size.x);
        top_right.y = 1 - (location_in_atlas.y / sub_texture_size.y);

        bottom_left.x = location_in_atlas.x * (1 / texture_size.x);
        bottom_left.y = 1 - ((location_in_atlas.y + sub_texture_size.y) / sub_texture_size.y);
    }
    
    void DeserializeSubTexture2D(SubTexture2D* sub_texture, const YAML::Node& node)
    {
        NIT_CHECK(sub_texture);
        sub_texture->name        = node["name"].as<String>();
        sub_texture->top_right   = node["top_right"].as<Vector2>();
        sub_texture->bottom_left = node["bottom_left"].as<Vector2>();
    }

    void SerializeSubTexture2D(const SubTexture2D* sub_texture, YAML::Emitter& emitter)
    {
        NIT_CHECK(sub_texture);
        emitter << YAML::Key << "name"        << YAML::Value << sub_texture->name;
        emitter << YAML::Key << "top_right"   << YAML::Value << sub_texture->top_right;
        emitter << YAML::Key << "bottom_left" << YAML::Value << sub_texture->bottom_left;
    }

    void RegisterTexture2DAsset()
    {
        RegisterEnumType<MinFilter>();
        RegisterEnumValue<MinFilter>("Linear", MinFilter::Linear);
        RegisterEnumValue<MinFilter>("Nearest",MinFilter::Nearest);
        
        RegisterEnumType<MagFilter>();
        RegisterEnumValue<MagFilter>("Linear", MagFilter::Linear);
        RegisterEnumValue<MagFilter>("Nearest",MagFilter::Nearest);

        RegisterEnumType<WrapMode>();
        RegisterEnumValue<WrapMode>("Repeat",     WrapMode::Repeat);
        RegisterEnumValue<WrapMode>("ClampToEdge",WrapMode::ClampToEdge);
        
        RegisterEnumType<TextureCoordinate>();
        RegisterEnumValue<TextureCoordinate>("U",TextureCoordinate::U);
        RegisterEnumValue<TextureCoordinate>("V",TextureCoordinate::V);
        
        RegisterAssetType<Texture2D>({
              LoadTexture2D
            , FreeTexture2D
            , SerializeTexture2D
            , DeserializeTexture2D
        });
    }

    i32 FindIndexOfSubTexture2D(const Texture2D* texture, const String& sub_texture_name)
    {
        NIT_CHECK(texture);
        for (u32 i = 0; i < texture->sub_texture_count; ++i)
        {
            if (texture->sub_textures[i].name == sub_texture_name)
            {
                return i;
            }
        }
        return -1;
    }

    void SerializeTexture2D(const Texture2D* texture, YAML::Emitter& emitter)
    {
        using namespace YAML;
        
        emitter << Key << "image_path"        << Value << texture->image_path;
        emitter << Key << "is_white_texture"  << Value << texture->is_white_texture;
        emitter << Key << "mag_filter"        << Value << GetStringFromEnumValue<MagFilter> (texture->mag_filter);
        emitter << Key << "min_filter"        << Value << GetStringFromEnumValue<MinFilter> (texture->min_filter);
        emitter << Key << "wrap_mode_u"       << Value << GetStringFromEnumValue<WrapMode>  (texture->wrap_mode_u);
        emitter << Key << "wrap_mode_u"       << Value << GetStringFromEnumValue<WrapMode>  (texture->wrap_mode_v);
        emitter << Key << "sub_texture_count" << Value << texture->sub_texture_count;
        
        if (texture->sub_textures)
        {
            emitter << Key << "sub_textures" << Value << BeginMap;
            for (u32 i = 0; i < texture->sub_texture_count; ++i)
            {
                SubTexture2D* sub_texture = &texture->sub_textures[i];
                emitter << Key << "sub_texture" << Value << BeginMap;
                SerializeSubTexture2D(sub_texture, emitter);
                emitter << EndMap;
            }
            emitter << EndMap;
        }
    }

    void DeserializeTexture2D(Texture2D* texture, const YAML::Node& node)
    {
        texture->image_path        = node["image_path"]                                     .as<String>();
        texture->is_white_texture  = node["is_white_texture"]                               .as<bool>();
        texture->mag_filter        = GetEnumValueFromString<MagFilter> (node["mag_filter"]  .as<String>());
        texture->min_filter        = GetEnumValueFromString<MinFilter> (node["min_filter"]  .as<String>());
        texture->wrap_mode_u       = GetEnumValueFromString<WrapMode>  (node["wrap_mode_u"] .as<String>());
        texture->wrap_mode_u       = GetEnumValueFromString<WrapMode>  (node["wrap_mode_u"] .as<String>());
        texture->sub_texture_count = node["sub_texture_count"]                              .as<u32>();
        
        const YAML::Node& sub_textures_node = node["sub_textures"];
        if (sub_textures_node && texture->sub_texture_count > 0)
        {
            texture->sub_textures = new SubTexture2D[texture->sub_texture_count];
            u32 sub_texture_index = 0;
            for (const auto& sub_texture_node_child : sub_textures_node)
            {
                const YAML::Node& sub_texture_node = sub_texture_node_child.second;
                SubTexture2D* sub_texture = &texture->sub_textures[sub_texture_index]; 
                DeserializeSubTexture2D(sub_texture, sub_texture_node);
                ++sub_texture_index;
            }
        }
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

        void* data_to_upload = !texture->is_white_texture ? (void*) texture->pixel_data : (void*) &WHITE_TEXTURE_DATA;

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
        glDeleteTextures(1, &texture->id);
        texture->id = 0;
        FreeTextureImage(texture);
        delete[] texture->sub_textures;
    }

    bool IsTexture2DValid(const Texture2D* texture)
    {
        return texture != nullptr && texture->id != 0;
    }

    void BindTexture2D(const Texture2D* texture, u32 slot)
    {
        NIT_CHECK(IsTexture2DValid(texture));
        glBindTextureUnit(slot, texture->id);
    }
}

#endif