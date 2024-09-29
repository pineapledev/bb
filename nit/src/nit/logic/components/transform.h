#pragma once

namespace Nit
{
    struct Transform
    {
        Vector3 position = V3_ZERO;
        Vector3 rotation = V3_ZERO;
        Vector3 scale    = V3_ONE;
    };

    Matrix4 ToMatrix4(const Transform& transform);
    Vector3 Up(const Transform& transform);
    Vector3 Right(const Transform& transform);
    Vector3 Front(const Transform& transform);
}