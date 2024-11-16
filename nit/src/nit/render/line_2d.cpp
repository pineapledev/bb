#include "nit_pch.h"
#include "line_2d.h"
#include "editor/editor_utils.h"
#include "entity/entity.h"

namespace nit
{
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
        component_register<Line2D>();
    }
}