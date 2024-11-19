#include "nit_pch.h"
#include "physic_material.h"
#include "core/asset.h"

#ifdef NIT_EDITOR_ENABLED
    #include "editor/editor_utils.h"
#endif

namespace nit
{
    void serialize(const PhysicMaterial* material, YAML::Emitter& emitter)
    {                                         
        emitter << YAML::Key << "density" << YAML::Value << material->density;
        emitter << YAML::Key << "friction" << YAML::Value << material->friction;
        emitter << YAML::Key << "bounciness" << YAML::Value << material->bounciness;
        emitter << YAML::Key << "threshold" << YAML::Value << material->threshold;
    }
    
    void deserialize(PhysicMaterial* material, const YAML::Node& node)
    {
        material->density              = node["density"]    .as<f32>();
        material->friction             = node["friction"]   .as<f32>();
        material->bounciness           = node["bounciness"] .as<f32>();
        material->threshold            = node["threshold"]  .as<f32>();
    }

#ifdef NIT_EDITOR_ENABLED
    void draw_editor(PhysicMaterial* material)
    {
        editor_draw_drag_f32("Density", material->density);
        editor_draw_drag_f32("Friction", material->friction);
        editor_draw_drag_f32("Bounciness", material->bounciness);
        editor_draw_drag_f32("Threshold", material->threshold);
    }
#endif
    
    void register_physic_material_asset()
    {
        asset_register_type<PhysicMaterial>({
            .fn_serialize   = serialize,
            .fn_deserialize = deserialize,
            NIT_IF_EDITOR_ENABLED(.fn_draw_editor = draw_editor)
        });
    }
}
