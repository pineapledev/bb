#pragma once
#include "nit/entity/entity.h"

namespace nit
{
    struct Rigidbody2D;
    
    struct WorldHandle
    {
        i16 index1   = 0;
        i16 revision = 0;
    };

    struct BodyHandle
    {
        i32 index1   = 0;
        i16 world0   = 0;
        i16 revision = 0;
    };

    struct ShapeHandle
    {
        i32 index1   = 0;
        i16 world0   = 0;
        i16 revision = 0;
    };
    
    struct Physics2D
    {
        WorldHandle world_handle        = {};
        Vector2     gravity             = { 0.f, -9.8f };
        u32         sub_steps           = 6u;
        EntityID*   all_entity_ids      = nullptr;
    };

    void       physics_2d_set_instance(Physics2D* instance);
    bool       physics_2d_has_instance();
    Physics2D* physics_2d_get_instance();
    void       physics_2d_init();
    void       physics_2d_finish();

    void rigidbody_add_force(Rigidbody2D& rb, const Vector2& force, const Vector2& point);
    void rigidbody_set_velocity(Rigidbody2D& rb, const Vector2& velocity);
    void rigidbody_set_angular_velocity(Rigidbody2D& rb, f32 velocity);

}
