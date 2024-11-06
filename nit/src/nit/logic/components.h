#pragma once
#include "nit/core/asset.h"
#include "nit/logic/entity.h"

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
    
    struct Name
    {
        String data;
    };

    bool IsEmpty(const Name& name);
    bool operator==(const Name& a, const Name& b);
    bool operator!=(const Name& a, const Name& b);
    bool Contains(const Name& name, const String& other);

    Entity FindEntityByName(const String& name);
    void FindEntitiesByName(Array<Entity>& entities, const String& name);
    
    void RegisterNameComponent();

    // UUID Declared in base
    Entity FindEntityByUUID(UUID uuid);
    void   FindEntitiesByUUID(Array<Entity>& entities, UUID uuid);
    
    void RegisterUUIDComponent();
    
    enum class CameraProjection : u8 { Perspective, Orthographic };
    
    struct Camera
    {
        CameraProjection projection = CameraProjection::Perspective;
        f32              aspect     = 1280.f / 720.f;
        f32              fov        = 100;
        f32              near_clip  = 0.1f;
        f32              far_clip   = 1000.f;
        f32              size       = 3.f; // Used in Orthographic
    };

    void RegisterCameraComponent();
    
    Matrix4 CalculateProjectionViewMatrix(const Camera& camera, const Transform& transform= { {0.f, 0.f, 3.f} });
    Matrix4 CalculateProjectionMatrix(const Camera& camera);
    Matrix4 CalculateViewMatrix(const Transform& transform);

    struct Font;
    
    struct Text
    {
        AssetHandle      font      = {};
        String           text      = "EMPTY TEXT";
        bool             visible   = true;
        Vector4          tint      = V4_ONE;
        f32              spacing   = 1.f;
        f32              size      = 1.f;
    };

    void RegisterTextComponent();

    struct Texture2D;
    
    struct Sprite
    {
        AssetHandle           texture            = {};
        String                sub_texture;
        i32                   sub_texture_index  = -1;
        bool                  visible            = true;
        Vector4               tint               = V4_ONE;
        Vector2               size               = V2_ONE;
        bool                  flip_x             = false;
        bool                  flip_y             = false;
        Vector2               tiling_factor      = V2_ONE;
        bool                  keep_aspect        = true;
    };

    void RegisterSpriteComponent();
    
    void SetSpriteSubTexture2D(Sprite& sprite, const String& sub_texture);
    void ResetSpriteSubTexture2D(Sprite& sprite);

    struct Circle
    {
        bool            visible                = true;
        Vector4         tint                   = V4_ONE;
        f32             radius                 = .5f;
        f32             thickness              = .05f;
        f32             fade                   = .01f;
    };

    void RegisterCircleComponent();

    struct Line2D
    {
        bool            visible          = true;
        Vector4         tint             = V4_ONE;
        Vector2         start            = V2_ZERO;
        Vector2         end              = V2_ONE;
        f32             thickness        = .05f;
    };

    void RegisterLine2DComponent();
}