#include "editor_utils.h"

#include <ranges>

#ifdef NIT_IMGUI_ENABLED
#include <imgui_internal.h>
#include "core/asset.h"

namespace nit
{
    using namespace ImGui;
    
    constexpr f32 COLUMN_WIDTH     = 200.f;
    constexpr Vector2 ITEM_SPACING = { 0.f, 6.f };
    constexpr Vector4 V4_COLOR_X   = {1.f, 0.3f, 0.3f, 1.f};
    constexpr Vector4 V4_COLOR_Y   = {0.368f, 0.737f, 0.521f, 1};
    constexpr Vector4 V4_COLOR_Z   = {0.3f, 0.3f, 1, 1};
    constexpr Vector4 V4_COLOR_W   = {0.43f, 0.258f, 0.96f, 1};

    bool property_in_context = false;
    
    void editor_begin_property(const char* label, u32 items)
    {
        PushID(label);

        Columns(2);
        SetColumnWidth(0, COLUMN_WIDTH);
        Text(label);
        NextColumn();

        PushMultiItemsWidths(items, CalcItemWidth());
        PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ ITEM_SPACING.x, ITEM_SPACING.y });

        property_in_context = true;
    }

    void editor_end_property()
    {
        PopStyleVar();
        Columns(1);
        PopID();
        property_in_context = false;
    }

    void editor_draw_spacing(u32 spacing)
    {
        for (u32 i = 0; i < spacing; i++)
            Spacing();
    }
    bool editor_draw_centered_button(const char* label, f32 alignment)
    {
        const ImGuiStyle& style = GetStyle();

        const f32 size = CalcTextSize(label).x + style.FramePadding.x * 2.0f;
        const f32 avail = GetContentRegionAvail().x;

        const f32 off = (avail - size) * alignment;
        if (off > 0.0f)
            SetCursorPosX(GetCursorPosX() + off);

        return Button(label);
    }

    void editor_draw_centered_text(const char* label, float alignment)
    {
        const ImGuiStyle& style = GetStyle();

        const f32 size = CalcTextSize(label).x + style.FramePadding.x * 2.0f;
        const f32 avail = GetContentRegionAvail().x;

        const f32 off = (avail - size) * alignment;
        if (off > 0.0f)
            SetCursorPosX(GetCursorPosX() + off);

        return Text(label);
    }

    bool editor_draw_input_text(const char* label, String& text)
    {
        editor_begin_property(label);
        static constexpr u32 MAX_CHARS = 300;
        char text_buffer[MAX_CHARS];
        strcpy_s(text_buffer, text.c_str());
        const bool text_changed = InputText("##text", text_buffer, sizeof(text_buffer));
        if (text_changed)
        {
            text = text_buffer;
        }
        editor_end_property();
        return text_changed;
    }

    bool editor_draw_input_folder(const Window* window, const char* label, String& text)
    {
        editor_begin_property(label);
        static constexpr u32 MAX_CHARS = 300;
        char text_buffer[MAX_CHARS];
        strcpy_s(text_buffer, text.c_str());
        bool text_changed = InputText("##text", text_buffer, sizeof(text_buffer));
        SameLine();

        if (Button("Find"))
        {
            const String path = SelectFolder(window, "", text);

            if (!path.empty())
            {
                strcpy_s(text_buffer, path.c_str());
                text_changed = true;
            }
        }

        if (text_changed)
        {
            text = text_buffer;
        }

        editor_end_property();
        return text_changed;
    }

    void editor_draw_text(const char* label, const char* text, ...)
    {
        editor_begin_property(label);
        va_list args;
        va_start(args, text);
        ImGui::TextV(text, args);
        va_end(args);
        editor_end_property();
    }

    bool editor_draw_bool(const char* label, bool& enabled)
    {
        editor_begin_property(label);
        bool changed = Checkbox("##", &enabled);
        if (property_in_context)
            PopItemWidth();
        editor_end_property();
        return changed;
    }

    bool editor_draw_drag_i32(const char* label, i32& num, f32 speed /*= 1.f*/)
    {
        editor_begin_property(label);
        bool changed = DragInt("##", &num, speed);
        if (property_in_context)
            PopItemWidth();
        editor_end_property();
        return changed;
    }

    bool editor_draw_drag_u32(const char* label, u32& num, f32 speed /*= 1.f*/)
    {
        editor_begin_property(label);
        int i = static_cast<int>(num);
        bool changed = DragInt("##", &i, speed);
        num = i;
        if (property_in_context)
            PopItemWidth();
        editor_end_property();
        return changed;
    }

    bool editor_draw_drag_f32(const char* label, f32& num, f32 speed, f32 min, f32 max)
    {
        editor_begin_property(label);
        bool changed = DragFloat("##", &num, speed, min, max, "%.3f");
        if (property_in_context)
            PopItemWidth();
        editor_end_property();
        return changed;
    }

    void editor_draw_combo(const char* label, String& selected, const Array<String>& options)
    {
        editor_begin_property(label);
        if (selected.empty())
            selected = options[0];

        if (BeginCombo("##combo", selected.c_str()))
        {
            for (const String& option : options)
            {
                const bool isSelected = selected == option;
                if (Selectable(option.c_str()))
                {
                    selected = option;
                }
                if (isSelected)
                    SetItemDefaultFocus();
            }

            EndCombo();
        }
        editor_end_property();
    }

    bool editor_draw_drag_f32_with_button(const char* label, f32& value, const Vector4& reset_color, f32 reset_value, f32 speed)
    {
        PushID(label);
        const f32 line_height = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.f;
        const ImVec2 button_size = { line_height + 3.f, line_height };

        PushStyleColor(ImGuiCol_Button, { reset_color.x, reset_color.y, reset_color.z, reset_color.w });

        const bool reset = Button(label, button_size);

        PopStyleColor();
        SameLine();
        const bool changed = DragFloat("##value", &value, speed, 0, 0, "%.3f");
        
        if (reset)
            value = reset_value;

        if (property_in_context)
            PopItemWidth();
        
        PopID();

        return changed || reset;
    }

    bool editor_draw_drag_vector2(const char* label, Vector2& vector, const Vector2& reset_value, f32 speed)
    {
        editor_begin_property(label, 2);
        bool x_changed = editor_draw_drag_f32_with_button("X", vector.x, V4_COLOR_X, reset_value.x, speed);
        SameLine();
        bool y_changed = editor_draw_drag_f32_with_button("Y", vector.y, V4_COLOR_Y, reset_value.y, speed);
        editor_end_property();
        return x_changed || y_changed;
    }

    bool editor_draw_drag_vector3(const char* label, Vector3& vector, const Vector3& reset_value, f32 speed)
    {
        editor_begin_property(label, 3);
        bool x_changed = editor_draw_drag_f32_with_button("X", vector.x, V4_COLOR_X, reset_value.x, speed);
        SameLine();
        bool y_changed = editor_draw_drag_f32_with_button("Y", vector.y, V4_COLOR_Y, reset_value.y, speed);
        SameLine();
        bool z_changed = editor_draw_drag_f32_with_button("Z", vector.z, V4_COLOR_Z, reset_value.z, speed);
        editor_end_property();
        return x_changed || y_changed || z_changed;
    }

    bool editor_draw_drag_vector4(const char* label, Vector4& vector, const Vector4& reset_value, f32 speed)
    {
        editor_begin_property(label, 4);
        bool x_changed = editor_draw_drag_f32_with_button("X", vector.x, V4_COLOR_X, reset_value.x, speed);
        SameLine();
        bool y_changed = editor_draw_drag_f32_with_button("Y", vector.y, V4_COLOR_Y, reset_value.y, speed);
        SameLine();
        bool z_changed = editor_draw_drag_f32_with_button("Z", vector.z, V4_COLOR_Z, reset_value.z, speed);
        SameLine();
        bool w_changed = editor_draw_drag_f32_with_button("W", vector.w, V4_COLOR_W, reset_value.w, speed);
        editor_end_property();
        return x_changed || y_changed || z_changed || w_changed;
    }

    bool editor_draw_color_palette(const char* label, Vector4& color)
    {
        editor_begin_property(label);
        f32 f32_color[] = { color.x, color.y, color.z, color.w };
        const bool changed = ColorEdit4("", f32_color, ImGuiColorEditFlags_NoLabel | ImGuiColorEditFlags_NoInputs);
        color = { f32_color[0], f32_color[1], f32_color[2], f32_color[3] };
        editor_end_property();
        return changed;
    }

    bool editor_draw_asset_combo(const char* label, Type* type, AssetHandle* asset)
    {
        String selected = asset->name;
        String prev = selected;
        
        Array<AssetHandle> assets;
        asset_find_by_type(type, assets);
        
        editor_begin_property(label);

        if (BeginCombo("##combo", selected.c_str()))
        {
            if (Selectable("None"))
            {
                selected = "";
            }
            
            for (auto& option : assets)
            {
                const bool is_selected = selected == option.name;
                if (Selectable(option.name.c_str()))
                {
                    selected = option.name;
                }
                if (is_selected)
                {
                    SetItemDefaultFocus();
                }
            }

            EndCombo();
        }
        *asset = asset_find_by_name(selected);
        
        editor_end_property();

        return prev != selected;
    }

    void editor_draw_resource_combo(const char* label, const Array<String>& extensions, String& selected)
    {
        editor_begin_property(label);

        Array<String> paths;
        paths.emplace_back("None");
        
        for (const auto& dir_entry : std::filesystem::recursive_directory_iterator(asset_get_directory()))
        {
            const Path& dir_path = dir_entry.path();
            const String relative_path = std::filesystem::relative(dir_path, asset_get_directory()).string();
            
            if (dir_entry.is_directory())
            {
                continue;
            }

            for (const String& extension : extensions)
            {
                if (dir_path.extension().string() == extension)
                {
                    paths.push_back(relative_path);
                }    
            }
        }
        
        if (selected.empty())
            selected = paths[0];

        if (BeginCombo("##combo", selected.c_str()))
        {
            for (const String& option : paths)
            {
                const bool is_selected = selected == option;
                if (Selectable(option.c_str()))
                {
                    selected = option;
                }
                if (is_selected)
                {
                    SetItemDefaultFocus();
                }
            }

            EndCombo();
        }
        
        editor_end_property();
    }
}

#endif