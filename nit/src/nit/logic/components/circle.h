﻿#pragma once

namespace Nit
{
    struct Circle
    {
        Vector4         tint                   = V4_ONE;
        f32             radius                 = .5f;
        f32             thickness              = .05f;
        f32             fade                   = .01f;
    };
}