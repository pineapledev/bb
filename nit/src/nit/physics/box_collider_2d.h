#pragma once
#include "nit/core/asset.h"

namespace nit
{
    struct BoxCollider2D
    {
        bool        is_trigger      = false;
        Vector2     size            = V2_ONE;
        Vector2     center          = V2_ZERO;
        AssetHandle physic_material = {};
        Vector2     prev_size       = V2_ONE;
        void*       fixture_ptr     = nullptr;
    };

    void register_box_collider_2d_component();
}