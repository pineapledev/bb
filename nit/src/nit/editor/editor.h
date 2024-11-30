#pragma once

#ifdef NIT_EDITOR_ENABLED
#include "nit/core/asset.h"
#include "nit/entity/entity.h"
#include "nit/render/frame_buffer.h"

namespace nit
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
            File = 0,
            Folder  ,
            Next    ,
            Pause   ,
            Play    ,
            Stop  
        };
        
        bool        enabled              = true;
        EntityID      editor_camera_entity = NULL_ENTITY;
        EntityID      selected_entity      = NULL_ENTITY;
        AssetHandle selected_asset       = {};
        Selection   selection            = Selection::None;
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

    struct EditorCameraController
    {
        f32     zoom_step              = 0.3f;
        f32     zoom_speed             = 5.f;
        f32     move_speed             = 2.f;

        bool    is_zooming             = false;
        f32     time_to_stop_zoom      = 0.2f;
        f32     time_zooming           = 0.f;
        f32     desired_zoom           = 0.f;
        Vector3 aux_position           = {};
        bool    mouse_down             = false;
        Vector3 offset_pos             = {};
        bool    is_right_mouse_pressed = false;
    };

    void    editor_set_instance(Editor* editor_instance);
    Editor* editor_get_instance();
    void    register_editor();
    void    editor_init();

    void    editor_begin();
    void    editor_end();
}

#endif