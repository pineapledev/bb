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
        using TConstantMap = TMap<TString, TUniquePtr<Constant>>;
        using TUsedConstantMap = TMap<TString, Constant*>;

        Material(const TSharedPtr<Shader>& shader, const TSharedPtr<MaterialData>& mat_data = {},
                 const TSharedPtr<Texture2D>& texture = {});
        ~Material() = default;

        const TSharedPtr<Shader>& GetShader() const { return shader; }
        const TSharedPtr<Texture2D>& GetTexture() const { return texture; }
        const TSharedPtr<MaterialData>& GetMaterialData() const { return material_data; }
        
        f32 GetConstantFloat(const TString& name);
        Vector3 GetConstantVec3(const TString& name);

        void SetConstantFloat(const TString& name, f32 value);
        void SetConstantVec2(const TString& name, const Vector2& value);
        void SetConstantVec3(const TString& name, const Vector3& value);
        void SetConstantVec4(const TString& name, const Vector4& value);
        void SetConstantMat4(const TString& name, const Matrix4& value);
        void SetConstantInt(const TString& name, i32 value);
        void SetConstantSampler2D(const TString& name, const i32* value, i32 size);
        void SubmitConstants();

        Constant* GetConstant(const TString& name);

        TSharedPtr<Shader> shader = nullptr;
        TSharedPtr<Texture2D> texture = nullptr;
        TConstantMap constants;
        TUsedConstantMap used_constants;
        TSharedPtr<MaterialData> material_data = nullptr;
    };
}