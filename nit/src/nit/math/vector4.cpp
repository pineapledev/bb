#include "vector4.h"
#include "matrix4.h"

namespace nit
{
    bool operator==(const Vector4& a, const Vector4& b)
    {
        return (Abs(a.x - b.x) <= F32_EPSILON) &&
               (Abs(a.y - b.y) <= F32_EPSILON) &&
               (Abs(a.z - b.z) <= F32_EPSILON) &&
               (Abs(a.w - b.w) <= F32_EPSILON);
    }

    bool operator!=(const Vector4& a, const Vector4& b)
    {
        return !(a == b);
    }

    Vector4 operator+(const Vector4& a, const Vector4& b)
    {
        return { a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w };
    }

    Vector4 operator-(const Vector4& a, const Vector4& b)
    {
        return { a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w };
    }

    Vector4 operator*(const Vector4& vector, f32 num)
    {
        return { vector.x * num, vector.y * num, vector.z * num, vector.w * num };
    }

    Vector4 operator/(const Vector4& vector, f32 num)
    {
        return { vector.x / num, vector.y / num, vector.z / num, vector.w / num };
    }

    Vector4& operator+=(Vector4& left, const Vector4& right)
    {
        left.x += right.x;
        left.y += right.y;
        left.z += right.z;
        left.w += right.w;
        return left;
    }

    Vector4& operator-=(Vector4& left, const Vector4& right)
    {
        left.x -= right.x;
        left.y -= right.y;
        left.z -= right.z;
        left.w -= right.w;
        return left;
    }

    Vector4& operator*=(Vector4& left, f32 num)
    {
        left.x *= num;
        left.y *= num;
        left.z *= num;
        left.w *= num;
        return left;
    }

    Vector4& operator/=(Vector4& left, f32 num)
    {
        left.x /= num;
        left.y /= num;
        left.z /= num;
        left.w /= num;
        return left;
    }

    Vector4& operator*(Vector4& left, const Matrix4& matrix)
    {
        left.x = left.x * matrix.m[0][0] + left.y * matrix.m[1][0] + left.z * matrix.m[2][0] + left.w * matrix.m[3][0];
        left.y = left.x * matrix.m[0][1] + left.y * matrix.m[1][1] + left.z * matrix.m[2][1] + left.w * matrix.m[3][1];
        left.z = left.x * matrix.m[0][2] + left.y * matrix.m[1][2] + left.z * matrix.m[2][2] + left.w * matrix.m[3][2];
        left.w = left.x * matrix.m[0][3] + left.y * matrix.m[1][3] + left.z * matrix.m[2][3] + left.w * matrix.m[3][3];
        return left;
    }
    
    template <>
    f32 Magnitude<Vector4>(const Vector4& val)
    {
        return std::sqrt(std::powf(val.x, 2) + std::powf(val.y, 2) + std::powf(val.z, 2) + std::powf(val.w, 2));
    }

    template <>
    Vector4 Normalize<Vector4>(const Vector4& val)
    {
        f32 mag = Magnitude(val);
        return { val.x / mag, val.y / mag, val.z / mag, val.w / mag };
    }

    template <>
    Vector4 Multiply<Vector4>(const Vector4& a, const Vector4& b)
    {
        Vector4 r;
        r.x = a.x * b.x;
        r.y = a.y * b.y;
        r.z = a.z * b.z;
        r.w = a.w * b.w;
        return r;
    }

    template <>
    Vector4 Divide<Vector4>(const Vector4& a, const Vector4& b)
    {
        Vector4 r;
        r.x = a.x / b.x;
        r.y = a.y / b.y;
        r.z = a.z / b.z;
        r.w = a.w / b.w;
        return r;
    }

    Vector4 GetRandomColor()
    {
        return {
            GetRandomValue(0.f, 1.f),
            GetRandomValue(0.f, 1.f),
            GetRandomValue(0.f, 1.f),
            1.f
        };
    }
}
