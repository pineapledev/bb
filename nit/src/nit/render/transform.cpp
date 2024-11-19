#include "transform.h"
#include "entity/entity.h"

#ifdef NIT_EDITOR_ENABLED
    #include "editor/editor_utils.h"
#endif

namespace nit
{
    Matrix4 transform_to_matrix(const Transform& transform)
    {
        return CreateTransform(transform.position, transform.rotation, transform.scale);
    }

    Vector3 transform_up(const Transform& transform)
    {
        return LookRotation(transform.rotation, V3_UP);
    }

    Vector3 transform_right(const Transform& transform)
    {
        return LookRotation(transform.rotation, V3_RIGHT);
    }

    Vector3 transform_front(const Transform& transform)
    {
        return LookRotation(transform.rotation, V3_FRONT);
    }
    
    void serialize(const Transform* transform, YAML::Emitter& emitter)
    {
        emitter << YAML::Key << "position" << YAML::Value << transform->position;
        emitter << YAML::Key << "rotation" << YAML::Value << transform->rotation;
        emitter << YAML::Key << "scale"    << YAML::Value << transform->scale;
    }

    void deserialize(Transform* transform, const YAML::Node& node)
    {
        transform->position = node["position"].as<Vector3>();
        transform->rotation = node["rotation"].as<Vector3>();
        transform->scale    = node["scale"].as<Vector3>();
    }

#ifdef NIT_EDITOR_ENABLED
    void draw_editor(Transform* transform)
    {
        editor_draw_drag_vector3("Position", transform->position);
        editor_draw_drag_vector3("Rotation", transform->rotation);
        editor_draw_drag_vector3("Scale", transform->scale);
    }
#endif
    
    void register_transform_component()
    {
        component_register<Transform>
        ({
            .fn_serialize   = serialize,
            .fn_deserialize = deserialize,
            NIT_IF_EDITOR_ENABLED(.fn_draw_editor = draw_editor)
        });
    }
}
