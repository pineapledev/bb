#pragma once
#include "nit/entity/entity.h"

#ifdef NIT_IMGUI_ENABLED

namespace nit
{
    struct AssetHandle;
}

namespace nit
{
    void editor_begin_property(const char* label, u32 items = 1);
    void editor_end_property();
    void editor_draw_spacing(u32 spacing);

    bool editor_draw_centered_button(const char* label, float alignment = 0.5f);
    void editor_draw_centered_text(const char* label, float alignment = 0.7f);

    bool editor_draw_input_text(const char* label, String& text);
    bool editor_draw_input_folder(const Window* window, const char* label, String& text);

    auto editor_draw_text(const char* label, const char* text, ...) -> void;
    bool editor_draw_bool(const char* label, bool& enabled);
    bool editor_draw_drag_i32(const char* label, i32& num, f32 speed = 1.f);
    bool editor_draw_drag_u32(const char* label, u32& num, f32 speed = 1.f);
    bool editor_draw_drag_f32(const char* label, f32& num, f32 speed = 0.1f, f32 min = 0.f, f32 max = 0.f);
    void editor_draw_combo(const char* label, String& selected, const Array<String>& options);
    bool editor_draw_drag_f32_with_button(const char* label, f32& value, const Vector4& reset_color = V4_COLOR_LIGHT_RED, f32 reset_value = 0.f, f32 speed = 0.05f);
    bool editor_draw_drag_vector2(const char* label, Vector2& vector, const Vector2& reset_value = V2_ZERO, f32 speed = 0.05f);
    bool editor_draw_drag_vector3(const char* label, Vector3& vector, const Vector3& reset_value = V3_ZERO, f32 speed = 0.05f);
    bool editor_draw_drag_vector4(const char* label, Vector4& vector, const Vector4& reset_value = V4_ZERO, f32 speed = 0.05f);
    bool editor_draw_color_palette(const char* label, Vector4& color);
    bool editor_draw_asset_combo(const char* label, Type* type, AssetHandle* asset);
    void editor_draw_resource_combo(const char* label, const Array<String>& extensions, String& selected);
    bool editor_draw_uuid_combo(const char* label, EntityID& entity_id, UUID& entity_uuid);
    
    template<typename T>
    bool editor_draw_enum_combo(const char* label, T& enum_data)
    {
        using namespace ImGui;
        
        EnumType* type = enum_type_get<T>();
        String selected = enum_to_string<T>(enum_data);
        String prev     = selected;
        
        editor_begin_property(label);

        if (BeginCombo("##combo", selected.c_str()))
        {
            for (auto& [index, name] : type->index_to_name)
            {
                const bool is_selected = selected == name;
                if (Selectable(name.c_str()))
                {
                    selected = name;
                }
                if (is_selected)
                {
                    SetItemDefaultFocus();
                }
            }

            EndCombo();
        }

        enum_data = enum_from_string<T>(selected);
        
        editor_end_property();

        return prev != selected;
    }
}

#endif