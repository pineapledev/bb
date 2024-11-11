#pragma once
#include "render_api.h"

namespace nit
{
    struct Constant
    {
        Constant(const String& name, ShaderDataType type, i32 size);
        Constant(Constant&& other) noexcept;
        Constant& operator=(Constant&& other) noexcept;

        ~Constant();
        
        union
        {
            i32* int_data;
            f32* float_data;
            void* data      = nullptr;
        };

        String name;
        ShaderDataType type;
        i32 size;
    };

    struct Shader
    {
        Shader() = default;
        ~Shader();

        void Compile(const char* vertex_source, const char* fragment_source);

        void GetConstantCollection(Array<UniquePtr<Constant>>& constants) const;

        void SetConstantFloat(const char* name, f32 value) const;
        void SetConstantVec2(const char* name, const f32* value) const;
        void SetConstantVec3(const char* name, const f32* value) const;
        void SetConstantVec4(const char* name, const f32* value) const;
        void SetConstantMat4(const char* name, const f32* value) const;
        void SetConstantInt(const char* name, i32 value) const;
        void SetConstantSampler2D(const char* name, const i32* value, i32 size) const;
        void SetConstantObjects(const char* name, int binding_point, int max_objects, int size_object,
                                const void* objects) const;

        void Bind() const;
        void Unbind() const;

        u32 id = 0;
        bool compiled = false;
    };
}
