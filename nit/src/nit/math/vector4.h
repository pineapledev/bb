#pragma once
#include "nit/math/math_common.h"

namespace nit
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
    inline Vector4 abs(const Vector4& val) { return { abs(val.x), abs(val.y), abs(val.z), abs(val.w)  }; }

    template<>
    f32 magnitude(const Vector4& val);
    
    template<>
    Vector4 normalize(const Vector4& val);

    template<>
    Vector4 multiply(const Vector4& a, const Vector4& b);

    template<>
    Vector4 divide(const Vector4& a, const Vector4& b);

    Vector4 GetRandomColor();
}

template<>
struct YAML::convert<nit::Vector4>
{
    static Node encode(const nit::Vector4& v)
    {
        Node node;
        node.push_back(v.x);
        node.push_back(v.y);
        node.push_back(v.z);
        node.push_back(v.w);
        node.SetStyle(EmitterStyle::Flow);
        return node;
    }

    static bool decode(const Node& node, nit::Vector4& v)
    {
        if (!node.IsSequence() || node.size() != 4)
            return false;
        
        v.x = node[0].as<float>();
        v.y = node[1].as<float>();
        v.z = node[2].as<float>();
        v.w = node[3].as<float>();
        return true;
    }
};

inline YAML::Emitter& operator<<(YAML::Emitter& out, const nit::Vector4& c)
{
    out << YAML::Flow;
    out << YAML::BeginSeq << c.x << c.y << c.z << c.w << YAML::EndSeq;
    return out;
}