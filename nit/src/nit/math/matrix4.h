#pragma once

namespace Nit
{
    struct Vector4;
    
    struct Matrix4 // COLUMN MAJOR
    {
        union
        {
            f32 m[4][4]; //i = column, j = row
            f32 n[16] = {
                1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f
            };
        };
    };

    bool operator==(const Matrix4& a, const Matrix4& b);
    
    bool operator!=(const Matrix4& a, const Matrix4& b);

    Matrix4 operator*(const Matrix4& a, const Matrix4& b);
    
    Matrix4& operator*=(Matrix4& a, const Matrix4& b);
    
    Vector4 operator*(const Matrix4& matrix, const Vector4& vector);
    
    void SetIdentity(Matrix4& matrix);
    
    void SetZero(Matrix4& matrix);
    
    Matrix4 CreateTransform(const Vector3& position, const Vector3& rotation = V3_ZERO, const Vector3& scale = V3_ONE);
    
    Matrix4 Translate(const Matrix4& matrix, const Vector3& translation);
    
    Matrix4 RotateX(const Matrix4& matrix, f32 x);
    
    Matrix4 RotateY(const Matrix4& matrix, f32 y);
    
    Matrix4 RotateZ(const Matrix4& matrix, f32 z);
    
    Matrix4 Rotate(const Matrix4& matrix, const Vector3& rotation);
    
    Matrix4 Scale(const Matrix4& matrix, const Vector3& scale);
    
    f32 Determinant(const Matrix4& matrix);
    
    Matrix4 Transpose(const Matrix4& matrix);
    
    Matrix4 Inverse(const Matrix4& matrix);
    
    Matrix4 OrthographicProjection(const f32 aspect_ratio, const f32 size, f32 near_plane, f32 far_plane);
    
    Matrix4 OrthographicProjection(f32 left, f32 right, f32 bottom, f32 top, f32 near_plane, f32 far_plane);
    
    Matrix4 ViewProjection(const Vector3& position, const Vector3& rotation);
    
    Matrix4 PerspectiveProjection(f32 fov, f32 aspect, f32 near_clip, f32 far_clip);

    bool Decompose(const Matrix4& matrix, Vector3& position, Vector3& rotation, Vector3& scale);
}