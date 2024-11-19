#include "nit_pch.h"
#include "box_collider_2d.h"
#include "entity/entity.h"

#ifdef NIT_EDITOR_ENABLED
#include "editor/editor_utils.h"
#endif

namespace nit
{
    void serialize(const BoxCollider2D* collider, YAML::Emitter& emitter)
    {
        emitter << YAML::Key << "size"            << YAML::Value << collider->size;
        emitter << YAML::Key << "center"          << YAML::Value << collider->center;
        emitter << YAML::Key << "physic_material" << YAML::Value << collider->physic_material;
    }
    
    void deserialize(BoxCollider2D* collider, const YAML::Node& node)
    {
        collider->size            = node["size"]            .as<Vector2>();
        collider->center          = node["center"]          .as<Vector2>();
        collider->physic_material = node["physic_material"] .as<AssetHandle>();
    }

#ifdef NIT_EDITOR_ENABLED
    void draw_editor(BoxCollider2D* collider)
    {
        editor_draw_drag_vector2("size", collider->size);
        editor_draw_drag_vector2("center", collider->center);
        //editor_draw_asset_combo("size", type_get<PhysicMaterial>(), collider->physic_material);
    }
#endif

    void register_box_collider_2d_component()
    {
        component_register<BoxCollider2D>({
            .fn_serialize   = serialize,
            .fn_deserialize = deserialize,
            NIT_IF_EDITOR_ENABLED(.fn_draw_editor = draw_editor)
        });
    }
}