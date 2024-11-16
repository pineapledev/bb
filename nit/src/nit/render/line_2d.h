#pragma once

namespace nit
{
    struct Line2D
    {
        bool            visible          = true;
        Vector4         tint             = V4_ONE;
        Vector2         start            = V2_ZERO;
        Vector2         end              = V2_ONE;
        f32             thickness        = .05f;
    };

    void register_line_2d_component();
}