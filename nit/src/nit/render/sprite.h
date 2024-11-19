#pragma once
#include "nit/core/asset.h"

namespace nit
{
    struct Texture2D;
    
    struct Sprite
    {
        AssetHandle           texture            = {};
        String                sub_texture;
        i32                   sub_texture_index  = -1;
        bool                  visible            = true;
        Vector4               tint               = V4_ONE;
        Vector2               size               = V2_ONE;
        bool                  flip_x             = false;
        bool                  flip_y             = false;
        Vector2               tiling_factor      = V2_ONE;
        bool                  keep_aspect        = true;
        i32                   draw_layer         = 0;
    };

    void register_sprite_component();
    
    void sprite_set_sub_texture(Sprite& sprite, const String& sub_texture);
    void sprite_reset_sub_texture(Sprite& sprite);
}
