#pragma once
#include "nit/core/asset.h"

struct CollisionCategory
{
    nit::AssetHandle collision_flags;
    String           name;
    u64              category    = 0;
    u64              mask        = 0;
    bool             invalidated = false;
};

void register_collision_category_component();