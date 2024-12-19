#include "transform.h"
#include "entity/entity.h"
#include "entity/entity_utils.h"

#ifdef NIT_EDITOR_ENABLED
    #include "editor/editor_utils.h"
#endif

namespace nit
{
    Matrix4 transform_to_matrix(Transform& transform)
    {
        const Matrix4 child_matrix = mat_create_transform(transform.position, transform.rotation, transform.scale);
        
        if (transform.parent_uuid.data == 0)
        {
            return child_matrix;
        }

        if (!entity_valid(transform.parent))
        {
            transform.parent = entity_find_by_uuid(transform.parent_uuid);
            if (!entity_valid(transform.parent) || !entity_has<Transform>(transform.parent))
            {
                transform.parent_uuid = {0};
                return child_matrix;
            }
        }
        
        const Transform& parent_transform = entity_get<Transform>(transform.parent);
        const Vector3& parent_pos = parent_transform.position;
        const Vector3& parent_rot = parent_transform.rotation;

        Vector3 child_pos = transform.position + parent_pos;
        child_pos = to_v3(rotate_around(to_v2(parent_pos), parent_rot.z, to_v2(child_pos)));
        
        Vector3 child_rot = transform.rotation + parent_rot;
        Vector3 child_scl = multiply(transform.scale, parent_transform.scale);
        
        return mat_create_transform(child_pos, child_rot, child_scl);
    }

    Vector3 transform_up(const Transform& transform)
    {
        return look_rotation(transform.rotation, V3_UP);
    }

    Vector3 transform_right(const Transform& transform)
    {
        return look_rotation(transform.rotation, V3_RIGHT);
    }

    Vector3 transform_front(const Transform& transform)
    {
        return look_rotation(transform.rotation, V3_FRONT);
    }
    
    void serialize(const Transform* transform, YAML::Emitter& emitter)
    {
        emitter << YAML::Key << "position"    << YAML::Value << transform->position;
        emitter << YAML::Key << "rotation"    << YAML::Value << transform->rotation;
        emitter << YAML::Key << "scale"       << YAML::Value << transform->scale;
        emitter << YAML::Key << "parent_uuid" << YAML::Value << (u64) transform->parent_uuid;
    }

    void deserialize(Transform* transform, const YAML::Node& node)
    {
        transform->position = node["position"].as<Vector3>();
        transform->rotation = node["rotation"].as<Vector3>();
        transform->scale    = node["scale"].as<Vector3>();
        
        if (auto& uuid_node = node["parent_uuid"])
        {
            transform->parent_uuid = UUID{uuid_node.as<u64>()};
        }
    }

#ifdef NIT_EDITOR_ENABLED
    void draw_editor(Transform* transform)
    {
        editor_draw_drag_vector3("Position", transform->position);
        editor_draw_drag_vector3("Rotation", transform->rotation);
        editor_draw_drag_vector3("Scale", transform->scale);
        
        editor_draw_uuid_combo("Parent", transform->parent, transform->parent_uuid);
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
