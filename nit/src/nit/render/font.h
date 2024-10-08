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
        String     font_path;
        void*      baked_char_data = nullptr;
        ID         font_atlas_id   = 0;
        Texture2D* atlas           = nullptr;
    };
    
    void RegisterFontAsset();
    void SerializeFont(const Font* font, YAML::Emitter& emitter);
    void DeserializeFont(Font* font, const YAML::Node& node);
    void LoadFont(Font* font);
    void FreeFont(Font* font);
    bool IsFontValid(const Font* font);
    void GetChar(const Font* font, char c, CharData& char_data);
}