#pragma once

namespace nit
{
    struct Circle
    {
        bool            visible                = true;
        Vector4         tint                   = V4_ONE;
        f32             radius                 = .5f;
        f32             thickness              = .05f;
        f32             fade                   = .01f;
    };

    void register_circle_component();
}
