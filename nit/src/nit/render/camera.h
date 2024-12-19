#pragma once
#include "transform.h"

namespace nit
{
    enum class CameraProjection : u8 { Perspective, Orthographic };
    
    struct Camera
    {
        CameraProjection projection = CameraProjection::Perspective;
        f32              aspect     = 1280.f / 720.f;
        f32              fov        = 100;
        f32              near_clip  = 0.1f;
        f32              far_clip   = 1000.f;
        f32              size       = 3.f; // Used in Orthographic
    };

    void register_camera_component();
    
    Matrix4 camera_proj_view(const Camera& camera, Transform transform = { {0.f, 0.f, 3.f} });
    Matrix4 camera_proj(const Camera& camera);
    Matrix4 camera_view(Transform& transform);
}