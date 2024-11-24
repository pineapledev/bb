#pragma once

namespace nit
{
    struct Physics2D
    {
        void*   world               = nullptr;
        Vector2 gravity             = { 0.f, -9.8f };
        u32     velocity_iterations = 6u;
        u32     position_iterations = 2u;
    };

    void       physics_2d_set_instance(Physics2D* instance);
    bool       physics_2d_has_instance();
    Physics2D* physics_2d_get_instance();
    void       physics_2d_init();
    void       physics_2d_finish();
}