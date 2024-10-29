#include "vector2.h"

namespace Nit
{
    bool operator==(const Vector2& a, const Vector2& b)
    {
        return (Abs(a.x - b.x) <= F32_EPSILON) && (Abs(a.y - b.y) <= F32_EPSILON);
    }

    bool operator!=(const Vector2& a, const Vector2& b)
    {
        return !(a == b);
    }

    Vector2 operator+(const Vector2& a, const Vector2& b)
    {
        return { a.x + b.x, a.y + b.y };
    }

    Vector2 operator-(const Vector2& a, const Vector2& b)
    {
        return { a.x - b.x, a.y - b.y };
    }

    Vector2 operator*(const Vector2& vector, f32 num)
    {
        return { vector.x * num, vector.y * num };
    }

    Vector2 operator/(const Vector2& vector, f32 num)
    {
        return { vector.x / num, vector.y / num };
    }

    Vector2& operator+=(Vector2& left, const Vector2& right)
    {
        left.x += right.x;
        left.y += right.y;
        return left;
    }

    Vector2& operator-=(Vector2& left, const Vector2& right)
    {
        left.x -= right.x;
        left.y -= right.y;
        return left;
    }

    Vector2& operator*=(Vector2& left, f32 num)
    {
        left.x *= num;
        left.y *= num;
        return left;
    }

    Vector2& operator/=(Vector2& left, f32 num)
    {
        left.x /= num;
        left.y /= num;
        return left;
    }

    f32 Angle(const Vector2& a, const Vector2& b)
    {
        const f32 cos_angle = Dot(a, b) / (Magnitude(a) * Magnitude(b));
        return std::acosf(cos_angle);
    }

    Vector2 RotateAround(Vector2 pivot, f32 angle, Vector2 point)
    {
        const f32 s = sin(ToRadians(angle));
        const f32 c = cos(ToRadians(angle));

        // translate point back to origin:
        point.x -= pivot.x;
        point.y -= pivot.y;

        // rotate point
        const f32 x_new = point.x * c - point.y * s;
        const f32 y_new = point.x * s + point.y * c;

        // translate point back:
        point.x = x_new + pivot.x;
        point.y = y_new + pivot.y;
        return point;
    }

    template<>
    Vector2 Abs<Vector2>(const Vector2& val)
    {
        return { Abs(val.x), Abs(val.y)  };
    }

    template<>
    f32 Magnitude<Vector2>(const Vector2& val)
    {
        return std::sqrt(std::powf(val.x, 2) + std::powf(val.y, 2));
    }

    template<>
    Vector2 Normalize<Vector2>(const Vector2& val)
    {
        f32 mag = Magnitude(val);
        return { val.x / mag, val.y / mag };
    }

    template<>
    Vector2 Multiply(const Vector2& a, const Vector2& b)
    {
        Vector2 r;
        r.x = a.x * b.x;
        r.y = a.y * b.y;
        return r;
    }

    template<>
    Vector2 Divide<Vector2>(const Vector2& a, const Vector2& b)
    {
        Vector2 r;
        r.x = a.x / b.x;
        r.y = a.y / b.y;
        return r;
    }
    
    template<>
    f32 Dot<Vector2>(const Vector2& a, const Vector2& b)
    {
        return a.x * b.x + a.y * b.y;
    }

    template<>
    f32 Distance<Vector2>(const Vector2& a, const Vector2& b)
    {
        return std::sqrtf(std::powf(a.x - b.x, 2) + std::powf(a.y - b.y, 2));
    }

    template <>
    Vector2 GetRandomValue<Vector2>(const Vector2& left, const Vector2& right)
    {
        return left + (right - left) * GetRandomValue(0.f, 1.f);
    }

    Vector2 ToVector2(const Vector3& value)
    {
        return { value.x, value.y };
    }
}
