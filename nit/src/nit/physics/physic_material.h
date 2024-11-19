#pragma once

namespace nit
{
    struct PhysicMaterial
    {
        f32 density    = 1.0f;
        f32 friction   = 0.5f;
        f32 bounciness = 0.0f;
        f32 threshold  = 0.5f;
    };
    
    void register_physic_material_asset();
}