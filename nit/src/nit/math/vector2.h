﻿#pragma once
#include "nit/math/math_common.h"

namespace Nit
{
    struct Vector2
    {
        f32 x = 0;
        f32 y = 0;
    };

    inline constexpr Vector2 V2_ZERO    = { 0,  0, };
    inline constexpr Vector2 V2_ONE     = { 1,  1, };
    inline constexpr Vector2 V2_RIGHT   = { 1,  0, };
    inline constexpr Vector2 V2_LEFT    = {-1,  0, };
    inline constexpr Vector2 V2_UP      = { 0,  1, };
    inline constexpr Vector2 V2_DOWN    = { 0, -1, };
    
    bool operator==(const Vector2& a, const Vector2& b);
    
    bool operator!=(const Vector2& a, const Vector2& b);
    
    Vector2 operator+(const Vector2& a, const Vector2& b);
    
    Vector2 operator-(const Vector2& a, const Vector2& b);
    
    Vector2 operator*(const Vector2& vector, f32 num);
    
    Vector2 operator/(const Vector2& vector, f32 num);

    Vector2& operator+=(Vector2& left, const Vector2& right);
    
    Vector2& operator-=(Vector2& left, const Vector2& right);

    Vector2& operator*=(Vector2& left, f32 num);

    Vector2& operator/=(Vector2& left, f32 num);
    
    f32 Angle(const Vector2& a, const Vector2& b);
    
    Vector2 RotateAround(Vector2 pivot, f32 angle, Vector2 point);
    
    template<>
    Vector2 Abs(const Vector2& val);

    template<>
    f32 Magnitude(const Vector2& val);

    template<>
    Vector2 Normalize(const Vector2& val);

    template<>
    Vector2 Multiply(const Vector2& a, const Vector2& b);

    template<>
    Vector2 Divide(const Vector2& a, const Vector2& b);

    template<>
    f32 Dot(const Vector2& a, const Vector2& b);

    template<>
    f32 Distance(const Vector2& a, const Vector2& b);
}