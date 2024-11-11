#pragma once
#include "texture.h"

namespace nit
{
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
        Texture2D  atlas;
    };
    
    void register_font_asset();
    void serialize_font(const Font* font, YAML::Emitter& emitter);
    void deserialize_font(Font* font, const YAML::Node& node);
    void load_font(Font* font);
    void free_font(Font* font);
    bool is_font_valid(const Font* font);
    void get_char(const Font* font, char c, CharData& char_data);
}