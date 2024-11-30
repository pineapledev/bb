#pragma once

namespace nit
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

    bool      operator==                  (const Matrix4& a, const Matrix4& b);
    bool      operator!=                  (const Matrix4& a, const Matrix4& b);
    Matrix4   operator*                   (const Matrix4& a, const Matrix4& b);
    Matrix4&  operator*=                  (Matrix4& a, const Matrix4& b);
    Vector4   operator*                   (const Matrix4& matrix, const Vector4& vector);

    void      mat_set_identity            (Matrix4& matrix);
    void      mat_set_zero                (Matrix4& matrix);
    Matrix4   mat_create_transform        (const Vector3& position, const Vector3& rotation = V3_ZERO, const Vector3& scale = V3_ONE);
    Matrix4   mat_translate               (const Matrix4& matrix, const Vector3& translation);
    Matrix4   mat_rotate_x                (const Matrix4& matrix, f32 x);
    Matrix4   mat_rotate_y                (const Matrix4& matrix, f32 y);
    Matrix4   mat_rotate_z                (const Matrix4& matrix, f32 z);
    Matrix4   mat_rotate                  (const Matrix4& matrix, const Vector3& rotation);
    Matrix4   mat_scale                   (const Matrix4& matrix, const Vector3& scale);
    f32       mat_determinant             (const Matrix4& matrix);
    Matrix4   mat_transpose               (const Matrix4& matrix);
    Matrix4   mat_inverse                 (const Matrix4& matrix);
    Matrix4   mat_ortho_projection        (const f32 aspect_ratio, const f32 size, f32 near_plane, f32 far_plane);
    Matrix4   mat_ortho_projection        (f32 left, f32 right, f32 bottom, f32 top, f32 near_plane, f32 far_plane);
    Matrix4   mat_view_projection         (const Vector3& position, const Vector3& rotation);
    Matrix4   mat_perspective_projection  (f32 fov, f32 aspect, f32 near_clip, f32 far_clip);
    bool      mat_decompose               (const Matrix4& matrix, Vector3& position, Vector3& rotation, Vector3& scale);
    Vector3   mat_screen_to_world         (const Matrix4& projection_view_matrix, const Vector2& screen_point, const Vector2& window_size);
}