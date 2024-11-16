#pragma once
#include "nit/core/asset.h"

namespace nit
{
    struct Text
    {
        AssetHandle      font      = {};
        String           text      = "EMPTY TEXT";
        bool             visible   = true;
        Vector4          tint      = V4_ONE;
        f32              spacing   = 1.f;
        f32              size      = 1.f;
    };

    void register_text_component();
}