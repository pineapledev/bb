#pragma once
#include "physics_2d.h"

namespace nit
{
    enum class BodyType { Static, Kinematic, Dynamic };
    
    struct Rigidbody2D
    {
        bool       enabled          = true;
        BodyType   body_type        = BodyType::Dynamic;
        float      mass             = 1.f;
        float      gravity_scale    = 0.f;
        bool       follow_transform = false;
        BodyHandle handle           = {};
        bool       invalidated      = false;
    };
    
    void register_rigidbody_2d_component();
}