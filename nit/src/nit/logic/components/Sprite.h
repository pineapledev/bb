#pragma once
#include "nit/render/texture.h"
#include "nit/render/primitives_2d.h"

namespace Nit
{
    struct Sprite
    {
        TSharedPtr<Texture2D> texture       = nullptr;
        Vector4               tint          = V4_ONE;
        Vector2               size          = V2_ONE;
        bool                  flip_x        = false;
        bool                  flip_y        = false;
        Vector2               tiling_factor = V2_ONE;
        bool                  keep_aspect   = true;
        QuadVertexData        vertex_data;
    };
}