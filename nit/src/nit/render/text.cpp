#include "nit_pch.h"
#include "text.h"
#include "font.h"
#include "entity/entity.h"

#ifdef NIT_EDITOR_ENABLED
    #include "editor/editor_utils.h"
#endif

namespace nit
{
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
        editor_draw_asset_combo("font", type_get<Font>(), &text->font);
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
        component_register<Text>();
    }
}