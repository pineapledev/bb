#pragma once
#include "nit/core/asset.h"

namespace nit
{
    struct CircleCollider
    {
        f32         radius          = 1.f;
        Vector2     center          = V2_ZERO;
        AssetHandle physic_material = {};
        f32         prev_radius     = 1.f;
        void*       fixture_ptr     = nullptr;
    };

    void register_circle_collider_component();
}