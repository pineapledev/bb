﻿#include "nit_pch.h"
#include "box_collider_2d.h"
#include "physic_material.h"
#include "entity/entity.h"

#ifdef NIT_EDITOR_ENABLED
#include "editor/editor_utils.h"
#endif

namespace nit
{
    void serialize(const BoxCollider2D* collider, YAML::Emitter& emitter)
    {
        emitter << YAML::Key << "is_trigger"      << YAML::Value << collider->is_trigger;
        emitter << YAML::Key << "size"            << YAML::Value << collider->size;
        emitter << YAML::Key << "center"          << YAML::Value << collider->center;
        emitter << YAML::Key << "physic_material" << YAML::Value << collider->physic_material;
    }
    
    void deserialize(BoxCollider2D* collider, const YAML::Node& node)
    {
        collider->is_trigger      = node["is_trigger"]      .as<bool>();
        collider->size            = node["size"]            .as<Vector2>();
        collider->center          = node["center"]          .as<Vector2>();
        collider->physic_material = node["physic_material"] .as<AssetHandle>();
    }

#ifdef NIT_EDITOR_ENABLED
    void draw_editor(BoxCollider2D* collider)
    {
        bool changed = false;
        changed |= editor_draw_bool("Is Trigger", collider->is_trigger);
        changed |= editor_draw_drag_vector2("Size", collider->size);
        changed |= editor_draw_drag_vector2("Center", collider->center);
        changed |= editor_draw_asset_combo("Physic Mat", type_get<PhysicMaterial>(), &collider->physic_material);

        if (changed)
        {
            collider->invalidated = false;
        }
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