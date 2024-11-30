#include "nit_pch.h"
#include "rigidbody_2d.h"
#include "entity/entity.h"

#ifdef NIT_EDITOR_ENABLED
#include "editor/editor_utils.h"
#endif

namespace nit
{
    void serialize(const Rigidbody2D* rigidbody, YAML::Emitter& emitter)
    {
        emitter << YAML::Key << "enabled"       << YAML::Value << rigidbody->enabled;
        emitter << YAML::Key << "body_type"     << YAML::Value << enum_to_string<BodyType>(rigidbody->body_type);
        emitter << YAML::Key << "mass"          << YAML::Value << rigidbody->mass;
        emitter << YAML::Key << "gravity_scale" << YAML::Value << rigidbody->gravity_scale;
    }
    
    void deserialize(Rigidbody2D* rigidbody, const YAML::Node& node)
    {
        rigidbody->enabled        = node["enabled"].as<bool>();
        rigidbody->body_type      = enum_from_string<BodyType>(node["body_type"].as<String>());
        rigidbody->mass           = node["mass"].as<f32>();
        rigidbody->gravity_scale  = node["gravity_scale"].as<f32>();
    }

#ifdef NIT_EDITOR_ENABLED
    void draw_editor(Rigidbody2D* rigidbody)
    {
        editor_draw_bool("Enabled", rigidbody->enabled);
        editor_draw_bool("Follow Transform", rigidbody->follow_transform);
        editor_draw_enum_combo("Body Type", rigidbody->body_type);
        editor_draw_drag_f32("Mass", rigidbody->mass);
        editor_draw_drag_f32("Gravity Scale", rigidbody->gravity_scale);
    }
#endif

    void register_rigidbody_2d_component()
    {
        enum_register<BodyType>();
        enum_register_value<BodyType>("Static", BodyType::Static);
        enum_register_value<BodyType>("Kinematic", BodyType::Kinematic);
        enum_register_value<BodyType>("Dynamic", BodyType::Dynamic);
        
        component_register<Rigidbody2D>({
            .fn_serialize   = serialize,
            .fn_deserialize = deserialize,
            NIT_IF_EDITOR_ENABLED(.fn_draw_editor = draw_editor)
        });
    }
}