#include "vector2.h"

namespace nit
{
    bool operator==(const Vector2& a, const Vector2& b)
    {
        return (abs(a.x - b.x) <= F32_EPSILON) && (abs(a.y - b.y) <= F32_EPSILON);
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

    f32 angle(const Vector2& a, const Vector2& b)
    {
        const f32 cos_angle = dot(a, b) / (magnitude(a) * magnitude(b));
        return std::acosf(cos_angle);
    }

    Vector2 rotate_around(Vector2 pivot, f32 angle, Vector2 point)
    {
        const f32 s = sin(to_radians(angle));
        const f32 c = cos(to_radians(angle));

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
    Vector2 abs<Vector2>(const Vector2& val)
    {
        return { abs(val.x), abs(val.y)  };
    }

    template<>
    f32 magnitude<Vector2>(const Vector2& val)
    {
        return std::sqrt(std::powf(val.x, 2) + std::powf(val.y, 2));
    }

    template<>
    Vector2 normalize<Vector2>(const Vector2& val)
    {
        f32 mag = magnitude(val);
        return { val.x / mag, val.y / mag };
    }

    template<>
    Vector2 multiply(const Vector2& a, const Vector2& b)
    {
        Vector2 r;
        r.x = a.x * b.x;
        r.y = a.y * b.y;
        return r;
    }

    template<>
    Vector2 divide<Vector2>(const Vector2& a, const Vector2& b)
    {
        Vector2 r;
        r.x = a.x / b.x;
        r.y = a.y / b.y;
        return r;
    }
    
    template<>
    f32 dot<Vector2>(const Vector2& a, const Vector2& b)
    {
        return a.x * b.x + a.y * b.y;
    }

    template<>
    f32 distance<Vector2>(const Vector2& a, const Vector2& b)
    {
        return std::sqrtf(std::powf(a.x - b.x, 2) + std::powf(a.y - b.y, 2));
    }

    Vector2 to_v2(const Vector3& value)
    {
        return { value.x, value.y };
    }

    Vector2 random_point_in_square(f32 x_min, f32 y_min, f32 x_max, f32 y_max)
    {
        std::random_device random_device;
        std::mt19937 random_engine(random_device());
        std::uniform_real_distribution distribution_x(x_min, x_max);
        std::uniform_real_distribution distribution_y(y_min, y_max);
        return {distribution_x(random_engine), distribution_y(random_engine)};
    }
}
