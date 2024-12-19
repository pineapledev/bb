﻿#pragma once
#include "nit/entity/entity.h"

namespace nit
{
    struct Transform
    {
        Vector3 position = V3_ZERO;
        Vector3 rotation = V3_ZERO;
        Vector3 scale    = V3_ONE;
        
        UUID     parent_uuid = {0};
        EntityID parent      = NULL_ENTITY;
    };
    
    Matrix4 transform_to_matrix(Transform& transform);
    Vector3 transform_up(const Transform& transform);
    Vector3 transform_right(const Transform& transform);
    Vector3 transform_front(const Transform& transform);

    void register_transform_component();
}
