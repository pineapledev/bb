#pragma once
#include "physics_2d.h"
#include "nit/core/asset.h"

namespace nit
{
    struct CircleCollider
    {
        bool        is_trigger      = false;
        f32         radius          = .5f;
        Vector2     center          = V2_ZERO;
        AssetHandle physic_material = {};
        ShapeHandle handle          = {};
        bool        invalidated     = false;
    };

    void register_circle_collider_component();
}