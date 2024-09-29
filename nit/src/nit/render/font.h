#pragma once

namespace Nit
{
    struct Texture2D;

    struct CharData
    {
        f32 x0, y0, s0, t0; // top-left
        f32 x1, y1, s1, t1; // bottom-right
        f32 x_pos, y_pos;
    };
    
    struct Font
    {
        Font() = default;
        Font(const char* file_path);
        Font(const Font& other) = delete;
        Font(Font&& other) noexcept;
        ~Font();

        Font& operator=(const Font& other) = delete;
        Font& operator=(Font&& other) noexcept;

        void Load(const char* file_path);
        void Free();

        void GetChar(char c, CharData& char_data) const;
        
        void* baked_char_data = nullptr;
        SharedPtr<Texture2D> atlas = nullptr;
    };
}