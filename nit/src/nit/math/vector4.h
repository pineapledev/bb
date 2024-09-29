﻿#pragma once
#include "nit/math/math_common.h"

namespace Nit
{
    struct Matrix4;
    
    struct Vector4
    {
        f32 x = 0;
        f32 y = 0;
        f32 z = 0;
        f32 w = 0;
    };

    inline constexpr Vector4 V4_ZERO               = {0.f, 0.f, 0.f, 0.f};
    inline constexpr Vector4 V4_ONE                = {1.f, 1.f, 1.f, 1.f};
    inline constexpr Vector4 V4_COLOR_WHITE        = {1.f, 1.f, 1.f, 1.f};
    inline constexpr Vector4 V4_COLOR_WHITE_FADED  = {1.f, 1.f, 1.f, 0.f};
    inline constexpr Vector4 V4_COLOR_BLACK        = {0.f, 0.f, 0.f, 1.f};
    inline constexpr Vector4 V4_COLOR_BLUE         = {0.f, 0.f, 1.f, 1.f};
    inline constexpr Vector4 V4_COLOR_LIGHT_BLUE   = {.3f, .3f, 1.f, 1.f};
    inline constexpr Vector4 V4_COLOR_CYAN         = {0.f, 1.f, 1.f, 1.f};
    inline constexpr Vector4 V4_COLOR_GRAY         = {.5f, .5f, .5f, 1.f};
    inline constexpr Vector4 V4_COLOR_DARK_GRAY    = {.2f, .2f, .2f, 1.f};
    inline constexpr Vector4 V4_COLOR_GREEN        = {0.f, 1.f, 0.f, 1.f};
    inline constexpr Vector4 V4_COLOR_LIGHT_GREEN  = {.3f, 1.f, .3f, 1.f};
    inline constexpr Vector4 V4_COLOR_GREY         = {.5f, .5f, .5f, 1.f};
    inline constexpr Vector4 V4_COLOR_DARK_GREY    = {.2f, .2f, .2f, 1.f};
    inline constexpr Vector4 V4_COLOR_MAGENTA      = {1.f, 0.f, 1.f, 1.f};
    inline constexpr Vector4 V4_COLOR_RED          = {1.f, 0.f, 0.f, 1.f};
    inline constexpr Vector4 V4_COLOR_LIGHT_RED    = {1.f, .3f, .3f, 1.f};
    inline constexpr Vector4 V4_COLOR_YELLOW       = {1.f, .92f, .016f, 1.f};
    inline constexpr Vector4 V4_COLOR_ORANGE       = {.97f, .60f, .11f, 1.f};

    bool operator==(const Vector4& a, const Vector4& b);
    
    bool operator!=(const Vector4& a, const Vector4& b);
    
    Vector4 operator+(const Vector4& a, const Vector4& b);
    
    Vector4 operator-(const Vector4& a, const Vector4& b);
    
    Vector4 operator*(const Vector4& vector, f32 num);
    
    Vector4 operator/(const Vector4& vector, f32 num);

    Vector4& operator+=(Vector4& left, const Vector4& right);
    
    Vector4& operator-=(Vector4& left, const Vector4& right);

    Vector4& operator*=(Vector4& left, f32 num);

    Vector4& operator/=(Vector4& left, f32 num);

    Vector4& operator*(Vector4& left, const Matrix4& matrix);
    
    template<>
    inline Vector4 Abs(const Vector4& val) { return { Abs(val.x), Abs(val.y), Abs(val.z), Abs(val.w)  }; }

    template<>
    f32 Magnitude(const Vector4& val);
    
    template<>
    Vector4 Normalize(const Vector4& val);

    template<>
    Vector4 Multiply(const Vector4& a, const Vector4& b);

    template<>
    Vector4 Divide(const Vector4& a, const Vector4& b);
}