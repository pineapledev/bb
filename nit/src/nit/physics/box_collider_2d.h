#pragma once
#include "physics_2d.h"
#include "nit/core/asset.h"

namespace nit
{
    struct BoxCollider2D
    {
        bool        is_trigger      = false;
        Vector2     size            = V2_ONE;
        Vector2     center          = V2_ZERO;
        AssetHandle physic_material = {};
        ShapeHandle handle          = {};
        bool        invalidated     = false;
    };

    void register_box_collider_2d_component();
}