#pragma once

namespace Nit
{
    struct MaterialData
    {
        f32     ambient_strength;
        f32     diffuse_strength;
        f32     specular_strength;
        f32     shininess;
        Vector3 color;
    };

    struct Constant;
    struct Shader;
    struct Texture2D;
    
    struct Material
    {
        using TConstantMap = Map<String, UniquePtr<Constant>>;
        using TUsedConstantMap = Map<String, Constant*>;

        Material(const SharedPtr<Shader>& shader, const SharedPtr<MaterialData>& mat_data = {});
        ~Material() = default;

        const SharedPtr<Shader>& GetShader() const { return shader; }
        const SharedPtr<MaterialData>& GetMaterialData() const { return material_data; }
        
        f32 GetConstantFloat(const String& name);
        Vector3 GetConstantVec3(const String& name);

        void SetConstantFloat(const String& name, f32 value);
        void SetConstantVec2(const String& name, const Vector2& value);
        void SetConstantVec3(const String& name, const Vector3& value);
        void SetConstantVec4(const String& name, const Vector4& value);
        void SetConstantMat4(const String& name, const Matrix4& value);
        void SetConstantInt(const String& name, i32 value);
        void SetConstantSampler2D(const String& name, const i32* value, i32 size);
        void SubmitConstants();

        Constant* GetConstant(const String& name);

        SharedPtr<Shader> shader = nullptr;
        TConstantMap constants;
        TUsedConstantMap used_constants;
        SharedPtr<MaterialData> material_data = nullptr;
    };
}