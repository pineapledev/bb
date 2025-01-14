#pragma once

constexpr u32 MAX_COLLISION_FLAGS = 64u;  

struct CollisionFlags
{
    u64    categories [MAX_COLLISION_FLAGS];
    String names      [MAX_COLLISION_FLAGS];
    u64    masks      [MAX_COLLISION_FLAGS];
    u32    count = 0;
};

u64 collision_flags_get_category_mask(CollisionFlags* flags, const String& mask_name);

void register_collision_flags_asset();