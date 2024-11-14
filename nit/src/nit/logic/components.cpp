#include "components.h"
#include "render/font.h"
#include "render/texture.h"

#ifdef NIT_EDITOR_ENABLED
#include "editor/editor_utils.h"
#endif

namespace nit
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

#ifdef NIT_EDITOR_ENABLED
    void DrawEditorTransform(Transform* transform)
    {
        editor_draw_drag_vector3("position", transform->position);
        editor_draw_drag_vector3("rotation", transform->rotation);
        editor_draw_drag_vector3("scale", transform->scale);
    }
#endif
    
    void register_transform_component()
    {
        RegisterComponentType<Transform>
        ({
            .fn_serialize   = SerializeTransform,
            .fn_deserialize = DeserializeTransform,
#ifdef NIT_EDITOR_ENABLED
            .fn_draw_editor = DrawEditorTransform
#endif
        });
    }

    bool IsEmpty(const Name& name)
    {
        return name.data.empty();
    }

    bool operator==(const Name& a, const Name& b)
    {
        return a.data == b.data;
    }

    bool operator!=(const Name& a, const Name& b)
    {
        return !(a == b);
    }

    bool Contains(const Name& name, const String& other)
    {
        return name.data.find(other) != String::npos;
    }

    Entity FindEntityByName(const String& name)
    {
        for (Entity entity : entity_get_group<Name>().entities)
        {
            if (entity_get<Name>(entity).data == name)
            {
                return entity;
            }
        }

        return NULL_ENTITY;
    }

    void FindEntitiesByName(Array<Entity>& entities, const String& name)
    {
        for (Entity entity : entity_get_group<Name>().entities)
        {
            if (entity_get<Name>(entity).data == name)
            {
                entities.push_back(entity);
            }
        }
    }

    void SerializeName(const Name* name, YAML::Emitter& emitter)
    {
        emitter << YAML::Key << "data" << YAML::Value << name->data;
    }

    void DeserializeName(Name* name, const YAML::Node& node)
    {
        name->data = node["data"].as<String>();
    }
    
#ifdef NIT_EDITOR_ENABLED
    void DrawEditorName(Name* name)
    {
        editor_draw_input_text("data", name->data);
        ImGui::Spacing();
    }
#endif

    void register_name_component()
    {
        RegisterComponentType<Name>
        ({
            .fn_serialize   = SerializeName,
            .fn_deserialize = DeserializeName,
#ifdef NIT_EDITOR_ENABLED
            .fn_draw_editor = DrawEditorName
#endif
        });

        entity_create_group<Name>();
    }

    bool IsValid(const UUID& uuid)
    {
        return uuid.data != 0;
    }

    bool operator==(const UUID& a, const UUID& b)
    {
        return a.data == b.data;
    }

    bool operator!=(const UUID& a, const UUID& b)
    {
        return !(a == b);
    }

    Entity FindEntityByUUID(UUID uuid)
    {
        for (Entity entity : entity_get_group<UUID>().entities)
        {
            if (entity_get<UUID>(entity) == uuid)
            {
                return entity;
            }
        }

        return NULL_ENTITY;
    }

    void FindEntitiesByUUID(Array<Entity>& entities, UUID uuid)
    {
        for (Entity entity : entity_get_group<UUID>().entities)
        {
            if (entity_get<UUID>(entity) == uuid)
            {
                entities.push_back(entity);
            }
        }
    }

    void SerializeUUID(const UUID* uuid, YAML::Emitter& emitter)
    {
        emitter << YAML::Key << "data" << YAML::Value << uuid->data;
    }

    void DeserializeUUID(UUID* uuid, const YAML::Node& node)
    {
        uuid->data = node["data"].as<u64>();
    }
    
#ifdef NIT_EDITOR_ENABLED
    void DrawEditorUUID(UUID* uuid)
    {
        editor_draw_text("data", "%llu", uuid->data);
        ImGui::Spacing();
    }
#endif
    
    void register_uuid_component()
    {
        RegisterComponentType<UUID>
        ({
            .fn_serialize   = SerializeUUID,
            .fn_deserialize = DeserializeUUID,
#ifdef NIT_EDITOR_ENABLED
            .fn_draw_editor = DrawEditorUUID
#endif
        });

        entity_create_group<UUID>();
    }

    void SerializeCamera(const Camera* camera, YAML::Emitter& emitter)
    {
        emitter << YAML::Key << "projection" << YAML::Value << GetStringFromEnumValue<CameraProjection>(camera->projection);
        emitter << YAML::Key << "fov"        << YAML::Value << camera->fov;
        emitter << YAML::Key << "near_clip"  << YAML::Value << camera->near_clip;
        emitter << YAML::Key << "far_clip"   << YAML::Value << camera->far_clip;
        emitter << YAML::Key << "size"       << YAML::Value << camera->size;
    }

    void DeserializeCamera(Camera* camera, const YAML::Node& node)
    {
        camera->projection = GetEnumValueFromString<CameraProjection>(node["projection"].as<String>());
        camera->fov        = node["fov"]        .as<f32>();
        camera->near_clip  = node["near_clip"]  .as<f32>();
        camera->far_clip   = node["far_clip"]   .as<f32>();
        camera->size       = node["size"]       .as<f32>();
    }

#ifdef NIT_EDITOR_ENABLED
    void DrawEditorCamera(Camera* camera)
    {
        editor_draw_enum_combo("projection", camera->projection);
        editor_draw_drag_f32("aspect", camera->aspect);
        editor_draw_drag_f32("fov",    camera->fov);
        editor_draw_drag_f32("near", camera->near_clip);
        editor_draw_drag_f32("far",    camera->far_clip);
        editor_draw_drag_f32("size",   camera->size);
    }
#endif
    
    void register_camera_component()
    {
        RegisterEnumType<CameraProjection>();
        RegisterEnumValue<CameraProjection>("Orthographic", CameraProjection::Orthographic);
        RegisterEnumValue<CameraProjection>("Perspective", CameraProjection::Perspective);
        
        TypeArgs<Camera> args;
        args.fn_serialize   = SerializeCamera;
        args.fn_deserialize = DeserializeCamera;
#ifdef NIT_EDITOR_ENABLED
        args.fn_draw_editor = DrawEditorCamera;
#endif
        type_register<Camera>(args);
        RegisterComponentType<Camera>();
    }

    Matrix4 camera_proj_view(const Camera& camera, const Transform& transform)
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
                proj = ortho_projection(camera.aspect, camera.size, camera.near_clip, camera.far_clip);
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

#ifdef NIT_EDITOR_ENABLED
    void DrawEditorText(Text* text)
    {
        editor_draw_asset_combo("font", GetType<Font>(), &text->font);
        editor_draw_input_text("text", text->text);
        editor_draw_bool("visible", text->visible);
        editor_draw_color_palette("tint", text->tint);
        editor_draw_drag_f32("spacing", text->spacing);
        editor_draw_drag_f32("size", text->size);
    }
#endif
    
    void register_text_component()
    {
        TypeArgs<Text> args;
        args.fn_serialize   = SerializeText;
        args.fn_deserialize = DeserializeText;
#ifdef NIT_EDITOR_ENABLED
        args.fn_draw_editor = DrawEditorText;
#endif
        type_register<Text>(args);
        RegisterComponentType<Text>();
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

#ifdef NIT_EDITOR_ENABLED
    void DrawEditorSprite(Sprite* sprite)
    {
        editor_draw_asset_combo("texture", GetType<Texture2D>(), &sprite->texture);
        
        editor_draw_input_text("sub_texture", sprite->sub_texture);

        if (asset_valid(sprite->texture))
        {
            SetSpriteSubTexture2D(*sprite, sprite->sub_texture);
        }
        else
        {
            sprite->sub_texture_index = -1;
        }
        
        editor_draw_bool("visible", sprite->visible);
        editor_draw_color_palette("tint", sprite->tint);
        editor_draw_drag_vector2("size", sprite->size);
        editor_draw_bool("flip_x", sprite->flip_x);
        editor_draw_bool("flip_y", sprite->flip_y);
        editor_draw_drag_vector2("tiling_factor", sprite->tiling_factor);
        editor_draw_bool("keep_aspect", sprite->keep_aspect);
    }
#endif

    void register_sprite_component()
    {
        TypeArgs<Sprite> args;
        args.fn_serialize   = SerializeSprite;
        args.fn_deserialize = DeserializeSprite;
#ifdef NIT_EDITOR_ENABLED
        args.fn_draw_editor = DrawEditorSprite;
#endif
        type_register<Sprite>(args);
        RegisterComponentType<Sprite>();
    }

    void SetSpriteSubTexture2D(Sprite& sprite, const String& sub_texture)
    {
        sprite.sub_texture = sub_texture;
        
        if (!asset_valid(sprite.texture))
        {
            sprite.sub_texture_index = -1;
            return;
        }

        i32 index = texture_2d_get_sub_tex_index(asset_get_data<Texture2D>(sprite.texture), sub_texture);
        sprite.sub_texture_index = index;
    }

    void ResetSpriteSubTexture2D(Sprite& sprite)
    {
        sprite.sub_texture = "";
        sprite.sub_texture_index = -1;
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

#ifdef NIT_EDITOR_ENABLED
    void DrawEditorCircle(Circle* circle)
    {
        editor_draw_bool("visible", circle->visible);
        editor_draw_color_palette("tint", circle->tint);
        editor_draw_drag_f32("radius", circle->radius, 0.01f);
        editor_draw_drag_f32("thickness", circle->thickness, 0.01f);
        editor_draw_drag_f32("fade", circle->fade, 0.01f);
    }
#endif

    void register_circle_component()
    {
        TypeArgs<Circle> args;
        args.fn_serialize   = SerializeCircle;
        args.fn_deserialize = DeserializeCircle;
#ifdef NIT_EDITOR_ENABLED
        args.fn_draw_editor = DrawEditorCircle;
#endif
        type_register<Circle>(args);
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

#ifdef NIT_EDITOR_ENABLED
    void DrawEditorLine(Line2D* line)
    {
        editor_draw_bool("visible", line->visible);
        editor_draw_color_palette("tint", line->tint);
        editor_draw_drag_vector2("start", line->start);
        editor_draw_drag_vector2("end", line->end);
        editor_draw_drag_f32("thickness", line->thickness, 0.01f);
    }
#endif

    void register_line_2d_component()
    {
        TypeArgs<Line2D> args;
        args.fn_serialize   = SerializeLine2D;
        args.fn_deserialize = DeserializeLine2D;
#ifdef NIT_EDITOR_ENABLED
        args.fn_draw_editor = DrawEditorLine;
#endif
        type_register<Line2D>(args);
        RegisterComponentType<Line2D>();
    }
}