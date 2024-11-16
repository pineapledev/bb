#include "camera.h"
#include "entity/entity.h"

#ifdef NIT_EDITOR_ENABLED
    #include "editor/editor_utils.h"
#endif

namespace nit
{
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
        component_register<Camera>();
    }

    Matrix4 camera_proj_view(const Camera& camera, const Transform& transform)
    {
        return camera_proj(camera) * camera_view(transform);
    }

    Matrix4 camera_proj(const Camera& camera)
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

    Matrix4 camera_view(const Transform& transform)
    {
        return Inverse(transform_to_matrix(transform));
    }
}