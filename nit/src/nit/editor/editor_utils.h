#pragma once

#ifdef NIT_IMGUI_ENABLED

namespace nit
{
    struct AssetHandle;
}

namespace ImGui
{
    using namespace nit;
    
    void BeginProperty(const char* label, u32 items = 1);
    void EndProperty();

    void DrawSpacing(u32 spacing);

    bool DrawCenteredButton(const char* label, float alignment = 0.5f);
    void DrawCenteredText(const char* label, float alignment = 0.7f);

    bool DrawInputText(const char* label, String& text);
    bool DrawInputFolder(const Window* window, const char* label, String& text);

    void DrawText(const char* label, const char* text, ...);
    bool DrawBool(const char* label, bool& enabled);
    bool DrawDragI32(const char* label, i32& num, f32 speed = 1.f);
    bool DrawDragU32(const char* label, u32& num, f32 speed = 1.f);
    bool DrawDragF32(const char* label, f32& num, f32 speed = 0.1f);
    void DrawCombo(const char* label, String& selected, const Array<String>& options);
    bool DrawDragF32WithButton(const char* label, f32& value, const Vector4& reset_color = V4_COLOR_LIGHT_RED, f32 reset_value = 0.f, f32 speed = 0.05f);
    bool DrawDragVector2(const char* label, Vector2& vector, const Vector2& reset_value = V2_ZERO, f32 speed = 0.05f);
    bool DrawDragVector3(const char* label, Vector3& vector, const Vector3& reset_value = V3_ZERO, f32 speed = 0.05f);
    bool DrawDragVector4(const char* label, Vector4& vector, const Vector4& reset_value = V4_ZERO, f32 speed = 0.05f);
    bool DrawColorPalette(const char* label, Vector4& color);
    void DrawAssetCombo(const char* label, Type* type, AssetHandle* asset);
    void DrawResourceCombo(const char* label, const Array<String>& extensions, String& selected);
    
    template<typename T>
    void DrawEnumCombo(const char* label, T& enum_data)
    {
        EnumType* type = GetEnumType<T>();
        String selected = GetStringFromEnumValue<T>(enum_data);

        BeginProperty(label);

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

        enum_data = GetEnumValueFromString<T>(selected);
        
        EndProperty();
    }
}

#endif