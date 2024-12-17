#pragma once
#include "nit/core/asset.h"

namespace nit
{
    struct FlipBook
    {
        struct Key
        {
            String name;
            i32 index = -1;
            f32 time =  0.f;
        };

        static constexpr u32 MAX_KEYS = 100u;
        
        AssetHandle texture;
        Key keys[MAX_KEYS];
        u32 key_count = 0u;
        f32 duration = 0.f;
    };
    
    void flipbook_sort(FlipBook* flipbook);
    void flipbook_adjust_to_duration(FlipBook* flipbook);
    void register_flipbook_asset();
    
    struct FlipBookAnimation
    {
        AssetHandle flipbook = {};
        bool loop = false;
        bool playing = false;
        u32 current_key = 0u;
        f32 time = 0.f;
    };
    
    void register_flipbook_animation_component();
}