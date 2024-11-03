#pragma once

#ifdef NIT_IMGUI_ENABLED

namespace Nit
{
    struct AssetHandle;
}

namespace ImGui
{
    using namespace Nit;
    
    void BeginProperty(const char* label, u32 items = 1);
    void EndProperty();

    void Spacing(u32 spacing);

    bool CenteredButton(const char* label, float alignment = 0.5f);
    void CenteredText(const char* label, float alignment = 0.7f);

    bool InputText(const char* label, String& text);
    bool InputFolder(const Window* window, const char* label, String& text);

    void Text(const char* label, const char* text, ...);
    bool Bool(const char* label, bool& enabled);
    bool DragI32(const char* label, i32& num, f32 speed = 1.f);
    bool DragU32(const char* label, u32& num, f32 speed = 1.f);
    bool DragF32(const char* label, f32& num, f32 speed = 0.1f);
    void Combo(const char* label, String& selected, const Array<String>& options);
    bool DragF32WithButton(const char* label, f32& value, const Vector4& reset_color = V4_COLOR_LIGHT_RED, f32 reset_value = 0.f, f32 speed = 0.05f);
    bool DragVector2(const char* label, Vector2& vector, const Vector2& reset_value = V2_ZERO, f32 speed = 0.05f);
    bool DragVector3(const char* label, Vector3& vector, const Vector3& reset_value = V3_ZERO, f32 speed = 0.05f);
    bool DragVector4(const char* label, Vector4& vector, const Vector4& reset_value = V4_ZERO, f32 speed = 0.05f);
    bool ColorPalette(const char* label, Vector4& color);
    void AssetCombo(const char* label, Type* type, AssetHandle* asset);

    template<typename T>
    void EnumCombo(const char* label, T& enum_data)
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
    
    bool IsOtherWindowFocused();
}

#endif