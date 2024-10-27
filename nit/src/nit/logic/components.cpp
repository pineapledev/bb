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
        return CalculateProjectionMatrix(camera) * CalculateViewMatrix(transform);
    }

    Matrix4 CalculateProjectionMatrix(const Camera& camera)
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
                proj = OrthographicProjection(camera.aspect, camera.size, camera.near_clip, camera.far_clip);
            }
            break;
        default:
            NIT_CHECK(false);
            break;
        }

        return proj;
    }

    Matrix4 CalculateViewMatrix(const Transform& transform)
    {
        return Inverse(ToMatrix4(transform));
    }

    void SerializeText(const Text* text, YAML::Emitter& emitter)
    {
        emitter << YAML::Key << "font"    << YAML::Value << text->font;
        emitter << YAML::Key << "text"    << YAML::Value << text->text;
        emitter << YAML::Key << "visible" << YAML::Value << text->visible;
        emitter << YAML::Key << "tint"    << YAML::Value << text->tint;
        emitter << YAML::Key << "spacing" << YAML::Value << text->spacing;
        emitter << YAML::Key << "size"    << YAML::Value << text->size;
    }
    
    void DeserializeText(Text* text, const YAML::Node& node)
    {
        text->font    = node["font"]    .as<AssetHandle>();
        text->text    = node["text"]    .as<String>();
        text->visible = node["visible"] .as<bool>();
        text->tint    = node["tint"]    .as<Vector4>();
        text->spacing = node["spacing"] .as<f32>();
        text->size    = node["size"]    .as<f32>();
    }
    
    void RegisterTextComponent()
    {
        TypeArgs<Text> args;
        args.fn_serialize   = SerializeText;
        args.fn_deserialize = DeserializeText;
        RegisterType<Text>(args);
        RegisterComponentType<Text>();
    }

    void AddFontToText(Text& text, AssetHandle& asset)
    {
        RemoveFontFromText(text);

        if (IsAssetValid(asset))
        {
            text.font = asset;
            text.font_data = GetAssetDataPtr<Font>(asset);
            RetainAsset(asset);
        }
    }

    void RemoveFontFromText(Text& text)
    {
        if (IsAssetValid(text.font))
        {
            text.font_data = nullptr;
            ReleaseAsset(text.font);
        }
    }

    void SerializeSprite(const Sprite* sprite, YAML::Emitter& emitter)
    {                                         
        emitter << YAML::Key << "texture"       << YAML::Value << sprite->texture;
        emitter << YAML::Key << "sub_texture"   << YAML::Value << sprite->sub_texture;
        emitter << YAML::Key << "visible"       << YAML::Value << sprite->visible;
        emitter << YAML::Key << "tint"          << YAML::Value << sprite->tint;
        emitter << YAML::Key << "size"          << YAML::Value << sprite->size;
        emitter << YAML::Key << "flip_x"        << YAML::Value << sprite->flip_x;
        emitter << YAML::Key << "flip_y"        << YAML::Value << sprite->flip_y;
        emitter << YAML::Key << "tiling_factor" << YAML::Value << sprite->tiling_factor;
        emitter << YAML::Key << "keep_aspect"   << YAML::Value << sprite->keep_aspect;
    }
    
    void DeserializeSprite(Sprite* sprite, const YAML::Node& node)
    {
        sprite->texture       = node["texture"]       .as<AssetHandle>();
        sprite->sub_texture   = node["sub_texture"]   .as<String>();
        sprite->visible       = node["visible"]       .as<bool>();
        sprite->tint          = node["tint"]          .as<Vector4>();
        sprite->size          = node["size"]          .as<Vector2>();
        sprite->flip_x        = node["flip_x"]        .as<bool>();
        sprite->flip_y        = node["flip_y"]        .as<bool>();
        sprite->tiling_factor = node["tiling_factor"] .as<Vector2>();
        sprite->keep_aspect   = node["keep_aspect"]   .as<bool>();
    }

    void RegisterSpriteComponent()
    {
        TypeArgs<Sprite> args;
        args.fn_serialize   = SerializeSprite;
        args.fn_deserialize = DeserializeSprite;
        RegisterType<Sprite>(args);
        RegisterComponentType<Sprite>();
    }

    void SetSpriteSubTexture2D(Sprite& sprite, const String& sub_texture)
    {
        sprite.sub_texture = sub_texture;
        
        if (!sprite.texture_data)
        {
            sprite.sub_texture_index = -1;
            return;
        }

        i32 index = FindIndexOfSubTexture2D(sprite.texture_data, sub_texture);
        sprite.sub_texture_index = index;
    }

    void ResetSpriteSubTexture2D(Sprite& sprite)
    {
        sprite.sub_texture = "";
        sprite.sub_texture_index = -1;
    }

    void AddTextureToSprite(Sprite& sprite, AssetHandle& asset)
    {
        if (sprite.texture_data)
        {
            RemoveTextureFromSprite(sprite);
        }
        
        if (IsAssetValid(asset))
        {
            sprite.texture = asset;
            sprite.texture_data = GetAssetDataPtr<Texture2D>(asset);
            SetSpriteSubTexture2D(sprite, sprite.sub_texture);
            RetainAsset(asset);
        }
    }

    void RemoveTextureFromSprite(Sprite& sprite)
    {
        if (IsAssetValid(sprite.texture))
        {
            sprite.texture_data = nullptr;
            ResetSpriteSubTexture2D(sprite);
            ReleaseAsset(sprite.texture);
        }
    }

    void SerializeCircle(const Circle* circle, YAML::Emitter& emitter)
    {                                         
        emitter << YAML::Key << "visible"   << YAML::Value << circle->visible;
        emitter << YAML::Key << "tint"      << YAML::Value << circle->tint;
        emitter << YAML::Key << "radius"    << YAML::Value << circle->radius;
        emitter << YAML::Key << "thickness" << YAML::Value << circle->thickness;
        emitter << YAML::Key << "fade"      << YAML::Value << circle->fade;
    }
    
    void DeserializeCircle(Circle* circle, const YAML::Node& node)
    {
        circle->visible   = node["visible"]   .as<bool>();
        circle->tint      = node["tint"]      .as<Vector4>();
        circle->radius    = node["radius"]    .as<f32>();
        circle->thickness = node["thickness"] .as<f32>();
        circle->fade      = node["fade"]      .as<f32>();
    }

    void RegisterCircleComponent()
    {
        TypeArgs<Circle> args;
        args.fn_serialize   = SerializeCircle;
        args.fn_deserialize = DeserializeCircle;
        RegisterType<Circle>(args);
        RegisterComponentType<Circle>();
    }

    void SerializeLine2D(const Line2D* line_2d, YAML::Emitter& emitter)
    {                                         
        emitter << YAML::Key << "visible"   << YAML::Value << line_2d->visible;
        emitter << YAML::Key << "tint"      << YAML::Value << line_2d->tint;
        emitter << YAML::Key << "start"     << YAML::Value << line_2d->start;
        emitter << YAML::Key << "end"       << YAML::Value << line_2d->end;
        emitter << YAML::Key << "thickness" << YAML::Value << line_2d->thickness;
    }
    
    void DeserializeLine2D(Line2D* line_2d, const YAML::Node& node)
    {
        line_2d->visible   = node["visible"]   .as<bool>();
        line_2d->tint      = node["tint"]      .as<Vector4>();
        line_2d->start     = node["start"]     .as<Vector2>();
        line_2d->end       = node["end"]       .as<Vector2>();
        line_2d->thickness = node["thickness"] .as<f32>();
    }

    void RegisterLine2DComponent()
    {
        TypeArgs<Line2D> args;
        args.fn_serialize   = SerializeLine2D;
        args.fn_deserialize = DeserializeLine2D;
        RegisterType<Line2D>(args);
        RegisterComponentType<Line2D>();
    }
}