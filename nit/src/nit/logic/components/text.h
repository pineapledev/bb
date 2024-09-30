﻿#pragma once
#include "nit/render/font.h"

namespace Nit
{
    struct Text
    {
        bool             visible = true;
        String           text    = "EMPTY TEXT";
        SharedPtr<Font>  font    = nullptr;
        Vector4          tint    = V4_ONE;
        f32              spacing = 1.f;
        f32              size    = 1.f;
    };
}