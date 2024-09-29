#include "material.h"
#include "shader.h"
#include "render_api.h"

namespace Nit
{
    Material::Material(const SharedPtr<Shader>& shader, const SharedPtr<MaterialData>& mat_data,
                       const SharedPtr<Texture2D>& texture)
        : shader(shader)
          , texture(texture)
          , material_data(mat_data)
    {
        if (!shader)
        {
            NIT_CHECK_MSG(false, "A valid shader pointer is required!");
            return;
        }

        if (!mat_data)
        {
            material_data = CreateSharedPtr<MaterialData>();
        }

        Array<UniquePtr<Constant>> shader_constants;
        shader->GetConstantCollection(shader_constants);
        const size_t num_of_constants = shader_constants.size();
        used_constants.reserve(num_of_constants);

        for (size_t i = 0; i < num_of_constants; ++i)
        {
            const String constant_name = shader_constants[i]->name;
            constants[constant_name] = std::move(shader_constants[i]);
        }
    }

    f32 Material::GetConstantFloat(const String& name)
    {
        const Constant* constant = GetConstant(name);
        if (!constant) { return 0; }
        return *constant->float_data;
    }

    Vector3 Material::GetConstantVec3(const String& name)
    {
        const Constant* constant = GetConstant(name);
        if (!constant) { return V3_ZERO; }
        f32* float_data = constant->float_data;
        return {float_data[0], float_data[1], float_data[2]};
    }

    void Material::SetConstantFloat(const String& name, f32 value)
    {
        Constant* constant = GetConstant(name);
        if (!constant) { return; }
        *constant->float_data = value;
        used_constants[name] = constant;
    }

    void Material::SetConstantVec2(const String& name, const Vector2& value)
    {
        Constant* constant = GetConstant(name);
        if (!constant) { return; }
        f32* float_data = constant->float_data;
        float_data[0] = value.x;
        float_data[1] = value.y;
        used_constants[name] = constant;
    }

    void Material::SetConstantVec3(const String& name, const Vector3& value)
    {
        Constant* constant = GetConstant(name);
        if (!constant) { return; }
        f32* float_data = constant->float_data;
        float_data[0] = value.x;
        float_data[1] = value.y;
        float_data[2] = value.z;
        used_constants[name] = constant;
    }

    void Material::SetConstantVec4(const String& name, const Vector4& value)
    {
        Constant* constant = GetConstant(name);
        if (!constant) { return; }
        f32* float_data = constant->float_data;
        float_data[0] = value.x;
        float_data[1] = value.y;
        float_data[2] = value.z;
        float_data[3] = value.w;
        used_constants[name] = constant;
    }

    void Material::SetConstantMat4(const String& name, const Matrix4& value)
    {
        Constant* constant = GetConstant(name);
        if (!constant) { return; }
        f32* float_data = constant->float_data;
        const f32* matrix_data = &(value.n[0]);

        for (i32 i = 0; i < 16; ++i)
        {
            float_data[i] = matrix_data[i];
        }

        used_constants[name] = constant;
    }

    void Material::SetConstantInt(const String& name, i32 value)
    {
        Constant* constant = GetConstant(name);
        if (!constant) { return; }
        *constant->int_data = value;
        used_constants[name] = constant;
    }

    void Material::SetConstantSampler2D(const String& name, const i32* value, i32 size)
    {
        Constant* constant = GetConstant(name);
        if (!constant) { return; }
        i32* float_data = constant->int_data;

        for (i32 i = 0; i < size; ++i)
        {
            float_data[i] = value[i];
        }

        used_constants[name] = constant;
    }

    void Material::SubmitConstants()
    {
        shader->Bind();

        for (auto& [key, constant] : used_constants)
        {
            if (!constant)
                break;

            const char* constant_name = constant->name.c_str();
            const ShaderDataType constant_type = constant->type;
            const i32* int_data = constant->int_data;
            const f32* float_data = constant->float_data;

            switch (constant_type)
            {
            default:
            case ShaderDataType::None:
            case ShaderDataType::Int2:
            case ShaderDataType::Int3:
            case ShaderDataType::Int4:
            case ShaderDataType::Mat3:
                NIT_CHECK_MSG(false, "Unsupported type!");
                break;

            case ShaderDataType::Int:
                shader->SetConstantInt(constant_name, *int_data);
                break;
            case ShaderDataType::Sampler2D:
                shader->SetConstantSampler2D(constant_name, int_data, GetSizeOfShaderDataType(constant_type));
                break;
            case ShaderDataType::Float:
                shader->SetConstantFloat(constant_name, *float_data);
                break;
            case ShaderDataType::Float2:
                shader->SetConstantVec2(constant_name, float_data);
                break;
            case ShaderDataType::Float3:
                shader->SetConstantVec3(constant_name, float_data);
                break;
            case ShaderDataType::Float4:
                shader->SetConstantVec4(constant_name, float_data);
                break;
            case ShaderDataType::Mat4:
                shader->SetConstantMat4(constant_name, float_data);
                break;
            case ShaderDataType::Bool:
                break;
            }
        }

        used_constants.clear();
    }

    Constant* Material::GetConstant(const String& name)
    {
        if (constants.contains(name))
        {
            return constants[name].get();
        }

        return nullptr;
    }
}
