#include "components.h"

#include "entity.h"
#include "render/font.h"
#include "render/texture.h"

namespace Nit
{
    Matrix4 ToMatrix4(const Transform& transform)
    {
        return CreateTransform(transform.position, transform.rotation, transform.scale);
    }

    Vector3 Up(const Transform& transform)
    {
        return LookRotation(transform.rotation, V3_UP);
    }

    Vector3 Right(const Transform& transform)
    {
        return LookRotation(transform.rotation, V3_RIGHT);
    }

    Vector3 Front(const Transform& transform)
    {
        return LookRotation(transform.rotation, V3_FRONT);
    }

    void SerializeTransform(const Transform* transform, YAML::Emitter& emitter)
    {
        emitter << YAML::Key << "position" << YAML::Value << transform->position;
        emitter << YAML::Key << "rotation" << YAML::Value << transform->rotation;
        emitter << YAML::Key << "scale"    << YAML::Value << transform->scale;
    }

    void DeserializeTransform(Transform* transform, const YAML::Node& node)
    {
        transform->position = node["position"].as<Vector3>();
        transform->rotation = node["rotation"].as<Vector3>();
        transform->scale    = node["scale"].as<Vector3>();
    }
    
    void RegisterTransformComponent()
    {
        TypeArgs<Transform> args;
        args.fn_serialize   = SerializeTransform;
        args.fn_deserialize = DeserializeTransform;
        RegisterType<Transform>(args);
        RegisterComponentType<Transform>();
    }

    Matrix4 CalculateProjectionViewMatrix(const Camera& camera, const Transform& transform)
    {
        Matrix4 proj;

        switch (camera.projection)
        {
        case CameraProjection::Perspective:
            {
                proj = PerspectiveProjection(camera.fov, camera.aspect, camera.near_clip, camera.far_clip);
            }
            break;
        case CameraProjection::Orthographic:
            {
                const f32 right = camera.aspect * camera.size;
                const f32 left = -right;
                proj = OrthographicProjection(left, right, -camera.size, camera.size, camera.near_clip, camera.far_clip);
            }
            break;
        default:
            NIT_CHECK(false);
            break;
        }

        return proj * Inverse(ToMatrix4(transform));
    }

    void AddFontToText(Text& text, ID font_id)
    {
        RemoveFontFromText(text);

        if (IsAssetValid(font_id))
        {
            text.font_id = font_id;
            text.font = GetAssetDataPtr<Font>(text.font_id);
            RetainAsset(text.font_id);
        }
    }

    void RemoveFontFromText(Text& text)
    {
        if (IsAssetValid(text.font_id))
        {
            text.font = nullptr;
            ReleaseAsset(text.font_id);
        }
    }

    void AddTextureToSprite(Sprite& sprite, ID texture_id)
    {
        RemoveTextureFromSprite(sprite);

        if (IsAssetValid(texture_id))
        {
            sprite.texture_id = texture_id;
            sprite.texture = GetAssetDataPtr<Texture2D>(sprite.texture_id);
            RetainAsset(sprite.texture_id);
        }
    }

    void RemoveTextureFromSprite(Sprite& sprite)
    {
        if (IsAssetValid(sprite.texture_id))
        {
            sprite.texture = nullptr;
            ReleaseAsset(sprite.texture_id);
        }
    }
}