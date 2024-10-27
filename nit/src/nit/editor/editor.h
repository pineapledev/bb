#pragma once

#ifdef NIT_EDITOR_ENABLED
#include "nit/logic/entity.h"
#include "nit/render/frame_buffer.h"

namespace Nit
{
    struct Editor
    {
        enum class Selection : u8
        {
            None,
            Entity,
            Asset
        };
        
        bool        enabled              = true;
        Entity      selected_entity      = 0;
        Selection   selection            = Selection::None;
        bool        show_viewport        = true;
        bool        show_sprite_packer   = false;
        bool        show_scene_entities  = true;
        bool        show_properties      = true;
        
        FrameBuffer frame_buffer       = {};
        Vector2     viewport_size;
        Vector2     viewport_min_bound;
        Vector2     viewport_max_bound;
        Vector2     mouse_position;
    };

    void SetEditorInstance(Editor* editor_instance);
    Editor* GetEditorInstance();

    void InitEditor();
    void BeginDrawEditor();
    void EndDrawEditor();
}

#endif