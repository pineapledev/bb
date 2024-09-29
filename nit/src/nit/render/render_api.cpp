#include "render_api.h"
#include "render_objects.h"

#ifdef NIT_GRAPHICS_API_OPENGL
#include <glad/glad.h>
#endif

namespace Nit
{
    ShaderDataType ShaderDataTypeFromOpenGL(i32 type)
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        switch (type)
        {
        case GL_FLOAT: return ShaderDataType::Float;
        case GL_FLOAT_VEC2: return ShaderDataType::Float2;
        case GL_FLOAT_VEC3: return ShaderDataType::Float3;
        case GL_FLOAT_VEC4: return ShaderDataType::Float4;
        case GL_FLOAT_MAT3: return ShaderDataType::Mat3;
        case GL_FLOAT_MAT4: return ShaderDataType::Mat4;
        case GL_INT: return ShaderDataType::Int;
        case GL_SAMPLER_2D: return ShaderDataType::Sampler2D;
        case GL_INT_VEC2: return ShaderDataType::Int2;
        case GL_INT_VEC3: return ShaderDataType::Int3;
        case GL_INT_VEC4: return ShaderDataType::Int4;
        case GL_BOOL: return ShaderDataType::Bool;
        default: return ShaderDataType::None;
        }
#else
        return ShaderDataType::None;
#endif
    }

    i32 ShaderDataTypeToOpenGL(ShaderDataType type)
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        switch (type)
        {
        case ShaderDataType::None: return 0;
        case ShaderDataType::Float: return GL_FLOAT;
        case ShaderDataType::Float2: return GL_FLOAT;
        case ShaderDataType::Float3: return GL_FLOAT;
        case ShaderDataType::Float4: return GL_FLOAT;
        case ShaderDataType::Mat3: return GL_FLOAT;
        case ShaderDataType::Mat4: return GL_FLOAT;
        case ShaderDataType::Int: return GL_INT;
        case ShaderDataType::Int2: return GL_INT;
        case ShaderDataType::Int3: return GL_INT;
        case ShaderDataType::Int4: return GL_INT;
        case ShaderDataType::Bool: return GL_BOOL;
        case ShaderDataType::Sampler2D: return GL_SAMPLER_2D;
        default: return 0;
        }
#else
        return 0;
#endif
    }

    u32 GetSizeOfShaderDataType(ShaderDataType type)
    {
        switch (type)
        {
        case ShaderDataType::Float: return 4;
        case ShaderDataType::Float2: return 4 * 2;
        case ShaderDataType::Float3: return 4 * 3;
        case ShaderDataType::Float4: return 4 * 4;
        case ShaderDataType::Mat3: return 4 * 3 * 3;
        case ShaderDataType::Mat4: return 4 * 4 * 4;
        case ShaderDataType::Int: return 4;
        //case ShaderDataType::Sampler2D: return 4 * 32;
        case ShaderDataType::Sampler2D: return 32;
        case ShaderDataType::Int2: return 4 * 2;
        case ShaderDataType::Int3: return 4 * 3;
        case ShaderDataType::Int4: return 4 * 4;
        case ShaderDataType::Bool: return 1;
        case ShaderDataType::None:
        default: return 0;
        }
    }

    u32 GetComponentCountFromShaderDataType(ShaderDataType type)
    {
        switch (type)
        {
        case ShaderDataType::None: return 0;
        case ShaderDataType::Float: return 1;
        case ShaderDataType::Float2: return 2;
        case ShaderDataType::Float3: return 3;
        case ShaderDataType::Float4: return 4;
        case ShaderDataType::Mat3: return 3 * 3;
        case ShaderDataType::Mat4: return 4 * 4;
        case ShaderDataType::Int: return 1;
        case ShaderDataType::Int2: return 2;
        case ShaderDataType::Int3: return 3;
        case ShaderDataType::Int4: return 4;
        case ShaderDataType::Bool: return 1;
        case ShaderDataType::Sampler2D: return 32;
        default: return 0;
        }
    }

    void* CreateFromShaderDataType(const ShaderDataType type)
    {
        switch (type)
        {
        case ShaderDataType::Float: return new f32;
        case ShaderDataType::Float2: return new f32[2];
        case ShaderDataType::Float3: return new f32[3];
        case ShaderDataType::Float4: return new f32[4];
        case ShaderDataType::Mat3: return new f32[9];
        case ShaderDataType::Mat4: return new f32[16];
        case ShaderDataType::Int: return new i32;
        case ShaderDataType::Sampler2D: return new i32[32];
        case ShaderDataType::Int2: return new i32[2];
        case ShaderDataType::Int3: return new i32[3];
        case ShaderDataType::Int4: return new i32[4];
        case ShaderDataType::Bool: return new bool;
        case ShaderDataType::None:
        default: return nullptr;
        }
    }
    
    void SetViewport(u32 x, u32 y, u32 width, u32 height)
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        glViewport(x, y, width, height);
#endif
    }

    void SetClearColor(const Vector4& clear_color)
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
#endif
    }

    void ClearScreen()
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#endif
    }

    void SetBlendingEnabled(bool enabled)
    {
        if (enabled)
        {
#ifdef NIT_GRAPHICS_API_OPENGL
            glEnable(GL_BLEND);
#endif
        }
        else
        {
#ifdef NIT_GRAPHICS_API_OPENGL
            glDisable(GL_BLEND);
#endif
        }
    }

    void SetBlendingMode(BlendingMode blending_mode)
    {
        switch (blending_mode)
        {
        case BlendingMode::Solid:
#ifdef NIT_GRAPHICS_API_OPENGL
            glBlendFunc(GL_ONE, GL_ZERO);
#endif
            break;
        case BlendingMode::Alpha:
#ifdef NIT_GRAPHICS_API_OPENGL
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#endif
            break;
        case BlendingMode::Add:
#ifdef NIT_GRAPHICS_API_OPENGL
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
#endif
            break;
        case BlendingMode::Multiply:
#ifdef NIT_GRAPHICS_API_OPENGL
            glBlendFunc(GL_DST_COLOR, GL_ZERO);
#endif
            break;
        }
    }

    void DrawElements(const SharedPtr<VertexArray>& vao, u32 element_count)
    {
        vao->Bind();
#ifdef NIT_GRAPHICS_API_OPENGL
        glDrawElements(GL_TRIANGLES, element_count, GL_UNSIGNED_INT, nullptr);
#endif
        vao->Unbind();
    }

    void DrawArrays(const SharedPtr<VertexArray>& vao, u32 element_count)
    {
        vao->Bind();
#ifdef NIT_GRAPHICS_API_OPENGL
        glDrawArrays(GL_TRIANGLES, 0, element_count);
#endif
        vao->Unbind();
    }

    void SetDepthTestEnabled(bool enabled)
    {
        if (enabled)
        {
#ifdef NIT_GRAPHICS_API_OPENGL
            glEnable(GL_DEPTH_TEST);
#endif
        }
        else
        {
#ifdef NIT_GRAPHICS_API_OPENGL
            glDisable(GL_DEPTH_TEST);
#endif
        }
    }
}