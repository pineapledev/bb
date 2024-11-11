#include "font.h"
#include "texture.h"
#include "nit/core/asset.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb/stb_truetype.h>

namespace nit
{
    void RegisterFontAsset()
    {
        register_asset_type<Font>({
              LoadFont
            , FreeFont
            , SerializeFont
            , DeserializeFont
        });
    }

    void SerializeFont(const Font* font, YAML::Emitter& emitter)
    {
        emitter << YAML::Key << "font_path" << YAML::Value << font->font_path;
    }

    void DeserializeFont(Font* font, const YAML::Node& node)
    {
        font->font_path = node["font_path"].as<String>();
    }

    //TODO: All of this is just garbage, pending to refactor. 
    void LoadFont(Font* font)
    {
        std::ifstream file_stream("assets/" + font->font_path, std::ifstream::binary);
        
        if (!file_stream)
        {
            NIT_CHECK_MSG(false, "Font not found!");
            return;
        }

        file_stream.seekg(0, std::ifstream::end);
        const std::streamoff length = file_stream.tellg();
        file_stream.seekg(0, std::ifstream::beg);
        const auto buffer = new char[length];
        file_stream.read(buffer, length);
        file_stream.close();

        const unsigned char* font_buffer = reinterpret_cast<unsigned char*>(buffer);

        static constexpr f32 PIXEL_HEIGHT = 64.f;
        static constexpr u32 HEIGHT = 1024;
        static constexpr u32 WIDTH = 1024;
        static constexpr u32 CHAR_COUNT = 256;

        constexpr i32 pixels_alpha_lenght = HEIGHT * WIDTH;
        const auto pixels_alpha = new unsigned char[pixels_alpha_lenght];
        auto* baked_char = new stbtt_bakedchar[CHAR_COUNT];

        stbtt_BakeFontBitmap(font_buffer, 0, PIXEL_HEIGHT, pixels_alpha, WIDTH, HEIGHT, 0, CHAR_COUNT, baked_char);
        font->baked_char_data = reinterpret_cast<void*>(baked_char);

        constexpr i32 pixels_rgb_lenght = pixels_alpha_lenght * 4;
        const auto pixels_rgb = new unsigned char[pixels_rgb_lenght];

        i32 curr_index = 0;
        i32 pixels_index = 0;
        for (i32 i = 0; i < pixels_rgb_lenght; i++)
        {
            curr_index++;
            if (curr_index == 4)
            {
                pixels_rgb[i] = pixels_alpha[pixels_index];
                pixels_index++;
                curr_index = 0;
                continue;
            }

            pixels_rgb[i] = 255;
        }
        
        font->atlas.pixel_data = pixels_rgb;
        font->atlas.size = { WIDTH, HEIGHT };
        font->atlas.channels   = 4;
        
        LoadTexture2D(&font->atlas);
        
        delete[] pixels_rgb;
        delete[] pixels_alpha;
        delete[] buffer;
    }

    void FreeFont(Font* font)
    {
        if (font->baked_char_data)
        {
            const auto* baked_char = static_cast<stbtt_bakedchar*>(font->baked_char_data);
            delete[] baked_char;
            font->baked_char_data = nullptr;
        }
    }

    bool IsFontValid(const Font* font)
    {
        return font->baked_char_data != nullptr;
    }

    void GetChar(const Font* font, char c, CharData& char_data)
    {
        const auto* baked_char = static_cast<stbtt_bakedchar*>(font->baked_char_data);
        f32 x_pos(0), y_pos(0);
        stbtt_aligned_quad quad;

        stbtt_GetBakedQuad(baked_char
            , static_cast<i32>(font->atlas.size.x)
            , static_cast<i32>(font->atlas.size.y)
            , c
            , &x_pos, &y_pos, &quad, true
        );

        char_data.x0 = quad.x0;
        char_data.x1 = quad.x1;
        char_data.y0 = quad.y0;
        char_data.y1 = quad.y1;
        char_data.s0 = quad.s0;
        char_data.s1 = quad.s1;
        char_data.t0 = quad.t0;
        char_data.t1 = quad.t1;
        char_data.x_pos = x_pos;
        char_data.y_pos = y_pos;
    }
}