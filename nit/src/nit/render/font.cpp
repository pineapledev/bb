#include "font.h"
#include "texture.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb/stb_truetype.h>

namespace Nit
{
    Font::Font(const char* file_path)
    {
        Load(file_path);
    }

    Font::Font(Font&& other) noexcept
        : baked_char_data(other.baked_char_data)
          , atlas(std::move(other.atlas))
    {
        other.baked_char_data = nullptr;
    }
    
    Font::~Font()
    {
        Free();
    }

    Font& Font::operator=(Font&& other) noexcept
    {
        baked_char_data = other.baked_char_data;
        other.baked_char_data = nullptr;
        atlas = other.atlas;
        return *this;
    }

    void Font::Load(const char* file_path)
    {
        if (baked_char_data)
        {
            Free();
        }

        std::ifstream file_stream(file_path, std::ifstream::binary);

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

        static constexpr f32 pixel_height = 64.f;
        static constexpr u32 height = 1024;
        static constexpr u32 width = 1024;
        static constexpr u32 char_count = 256;

        constexpr i32 pixels_alpha_lenght = height * width;
        const auto pixels_alpha = new unsigned char[pixels_alpha_lenght];
        auto* baked_char = new stbtt_bakedchar[char_count];

        stbtt_BakeFontBitmap(font_buffer, 0, pixel_height, pixels_alpha, width, height, 0, char_count, baked_char);
        baked_char_data = reinterpret_cast<void*>(baked_char);

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

        atlas = CreateSharedPtr<Texture2D>();
        atlas->UploadToGPU(pixels_rgb, width, height, 4);

        delete[] pixels_rgb;
        delete[] pixels_alpha;
        delete[] buffer;
    }

    void Font::Free()
    {
        atlas.reset();

        if (baked_char_data)
        {
            const auto* baked_char = static_cast<stbtt_bakedchar*>(baked_char_data);
            delete[] baked_char;
            baked_char_data = nullptr;
        }
    }
    
    void Font::GetChar(char c, CharData& char_data) const
    {
        NIT_CHECK_MSG(atlas, "Missing Atlas!");
        const auto* baked_char = static_cast<stbtt_bakedchar*>(baked_char_data);
        f32 x_pos(0), y_pos(0);
        stbtt_aligned_quad quad;
        stbtt_GetBakedQuad(baked_char, static_cast<i32>(atlas->size.x), static_cast<i32>(atlas->size.y), c,
                           &x_pos, &y_pos, &quad, true);
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
