#pragma once
#include "Transform.h"

namespace Nit
{
    enum class CameraProjection : u8 { Perspective, Orthographic };
    
    struct Camera
    {
        CameraProjection projection = CameraProjection::Perspective;
        f32        aspect           = 1280.f / 720.f;
        f32        fov              = 100;
        f32        near_clip        = 0.1f;
        f32        far_clip         = 1000.f;
        f32        size             = 3.f; // Used in Orthographic
    };

    Matrix4 CalculateProjectionViewMatrix(const Camera& camera, const Transform& transform= { {0.f, 0.f, 3.f} });
}