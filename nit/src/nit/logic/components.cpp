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

    void SerializeCamera(const Camera* camera, YAML::Emitter& emitter)
    {
        emitter << YAML::Key << "projection" << YAML::Value << GetStringFromEnumValue<CameraProjection>(camera->projection);
        emitter << YAML::Key << "aspect"     << YAML::Value << camera->aspect;
        emitter << YAML::Key << "fov"        << YAML::Value << camera->fov;
        emitter << YAML::Key << "near_clip"  << YAML::Value << camera->near_clip;
        emitter << YAML::Key << "far_clip"   << YAML::Value << camera->far_clip;
        emitter << YAML::Key << "size"       << YAML::Value << camera->size;
    }

    void DeserializeCamera(Camera* camera, const YAML::Node& node)
    {
        camera->projection = GetEnumValueFromString<CameraProjection>(node["projection"].as<String>());
        camera->aspect     = node["aspect"]     .as<f32>();
        camera->fov        = node["fov"]        .as<f32>();
        camera->near_clip  = node["near_clip"]  .as<f32>();
        camera->far_clip   = node["far_clip"]   .as<f32>();
        camera->size       = node["size"]       .as<f32>();
    }
    
    void RegisterCameraComponent()
    {
        RegisterEnumType<CameraProjection>();
        RegisterEnumValue<CameraProjection>("Orthographic", CameraProjection::Orthographic);
        RegisterEnumValue<CameraProjection>("Perspective", CameraProjection::Perspective);
        
        TypeArgs<Camera> args;
        args.fn_serialize   = SerializeCamera;
        args.fn_deserialize = DeserializeCamera;
        RegisterType<Camera>(args);
        RegisterComponentType<Camera>();
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

    void SerializeText(const Text* text, YAML::Emitter& emitter)
    {
        emitter << YAML::Key << "visible"     << YAML::Value << text->visible;
        emitter << YAML::Key << "text"        << YAML::Value << text->text;
        emitter << YAML::Key << "font_id"     << YAML::Value << text->font_id;
        emitter << YAML::Key << "tint"        << YAML::Value << text->tint;
        emitter << YAML::Key << "spacing"     << YAML::Value << text->spacing;
        emitter << YAML::Key << "size"        << YAML::Value << text->size;
    }
    
    void DeserializeText(Text* text, const YAML::Node& node)
    {
        text->visible  = node["visible"] .as<bool>();
        text->text     = node["text"]    .as<String>();
        text->font_id  = node["font_id"] .as<ID>();
        text->tint     = node["tint"]    .as<Vector4>();
        text->spacing  = node["spacing"] .as<f32>();
        text->size     = node["size"]    .as<f32>();
    }
    
    void RegisterTextComponent()
    {
        TypeArgs<Text> args;
        args.fn_serialize   = SerializeText;
        args.fn_deserialize = DeserializeText;
        RegisterType<Text>(args);
        RegisterComponentType<Text>();
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