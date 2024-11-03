#pragma once

#ifdef NIT_EDITOR_ENABLED
#include "nit/core/asset.h"
#include "nit/logic/entity.h"
#include "nit/render/frame_buffer.h"

namespace Nit
{
    struct AssetNode
    {
        bool             is_dir   = false;
        String           path     = {};
        u32              parent   = U32_MAX;
        AssetHandle      asset    = {};
        Array<u32>       children = {};
    };
    
    struct Editor
    {
        enum class Selection : u8
        {
            None,
            Entity,
            Asset
        };

        enum class Icon : u8
        {
            File   = 0,
            Folder = 1
        };
        
        bool        enabled              = true;
        Entity      selected_entity      = 0;
        AssetHandle selected_asset       = {};
        Selection   selection            = Selection::Entity;
        bool        show_viewport        = true;
        bool        show_sprite_packer   = false;
        bool        show_scene_entities  = true;
        bool        show_properties      = true;
        bool        show_assets          = true;
        bool        show_stats           = false;
        
        Pool             asset_nodes;
        u32              root_node = U32_MAX;
        u32              draw_node = U32_MAX;
        AssetHandle      icons;
        FrameBuffer      frame_buffer;
        Vector2          viewport_size;
        Vector2          viewport_min_bound;
        Vector2          viewport_max_bound;
        Vector2          mouse_position;
    };

    void SetEditorInstance(Editor* editor_instance);
    Editor* GetEditorInstance();
    
    void InitEditor();
    void BeginDrawEditor();
    void EndDrawEditor();
}

#endif