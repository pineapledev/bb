#include "nit_pch.h"
#include "circle.h"

#include "entity/entity.h"

#ifdef NIT_EDITOR_ENABLED
    #include "editor/editor_utils.h"
#endif

namespace nit
{
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
        component_register<Circle>();
    }
}
