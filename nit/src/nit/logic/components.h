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

    void RegisterTransformComponent();
    
    enum class CameraProjection : u8 { Perspective, Orthographic };
    
    struct Camera
    {
        CameraProjection projection = CameraProjection::Perspective;
        f32        aspect           = 1280.f / 720.f;
        f32        fov              = 100;
        f32        near_clip        = 0.1f;
        f32        far_clip         = 1000.f;
        f32        size             = 3.f; // Used in Orthographic
    };

    Matrix4 CalculateProjectionViewMatrix(const Camera& camera, const Transform& transform= { {0.f, 0.f, 3.f} });

    struct Font;
    
    struct Text
    {
        bool             visible = true;
        String           text    = "EMPTY TEXT";
        Font*            font    = nullptr;
        ID               font_id = 0;
        Vector4          tint    = V4_ONE;
        f32              spacing = 1.f;
        f32              size    = 1.f;
    };

    void AddFontToText(Text& text, ID font_id);
    void RemoveFontFromText(Text& text);

    struct Texture2D;
    
    struct Sprite
    {
        bool                  visible          = true;
        Texture2D*            texture          = nullptr;
        ID                    texture_id       = 0;
        Vector4               tint             = V4_ONE;
        Vector2               size             = V2_ONE;
        bool                  flip_x           = false;
        bool                  flip_y           = false;
        Vector2               tiling_factor    = V2_ONE;
        bool                  keep_aspect      = true;
    };

    void AddTextureToSprite(Sprite& sprite, ID texture_id);
    void RemoveTextureFromSprite(Sprite& sprite);

    struct Circle
    {
        bool            visible                = true;
        Vector4         tint                   = V4_ONE;
        f32             radius                 = .5f;
        f32             thickness              = .05f;
        f32             fade                   = .01f;
    };

    struct Line2D
    {
        bool            visible          = true;
        Vector4         tint             = V4_ONE;
        Vector2         start            = V2_ZERO;
        Vector2         end              = V2_ONE;
        f32             thickness        = .05f;
    };
}