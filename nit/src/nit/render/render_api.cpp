#include "render_api.h"
#include "render_objects.h"

#ifdef NIT_GRAPHICS_API_OPENGL
#include <glad/glad.h>

namespace nit
{
    ShaderDataType shader_data_type_from_open_gl(i32 type)
    {
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
    }

    i32 shader_data_type_to_open_gl(ShaderDataType type)
    {
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
    }

    u32 get_size_of_shader_data_type(ShaderDataType type)
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

    u32 get_component_count_from_shader_data_type(ShaderDataType type)
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

    void* create_from_shader_data_type(const ShaderDataType type)
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

    void set_viewport(const Vector2& size)
    {
        glViewport(0, 0, (i32) size.x, (i32) size.y);
    }

    void set_viewport(u32 x, u32 y, u32 width, u32 height)
    {
        glViewport(x, y, width, height);
    }

    void set_clear_color(const Vector4& clear_color)
    {
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    }

    void clear_screen()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void set_blending_enabled(bool enabled)
    {
        if (enabled)
        {
            glEnable(GL_BLEND);
        }
        else
        {
            glDisable(GL_BLEND);
        }
    }

    void set_blending_mode(BlendingMode blending_mode)
    {
        switch (blending_mode)
        {
        case BlendingMode::Solid:
            glBlendFunc(GL_ONE, GL_ZERO);
            break;
        case BlendingMode::Alpha:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            break;
        case BlendingMode::Add:
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            break;
        case BlendingMode::Multiply:
            glBlendFunc(GL_DST_COLOR, GL_ZERO);
            break;
        }
    }

    void draw_elements(u32 vao, u32 element_count)
    {
        bind_vertex_array(vao);
        glDrawElements(GL_TRIANGLES, element_count, GL_UNSIGNED_INT, nullptr);
        unbind_vertex_array();
    }

    void draw_arrays(u32 vao, u32 element_count)
    {
        bind_vertex_array(vao);
        glDrawArrays(GL_TRIANGLES, 0, element_count);
        unbind_vertex_array();
    }

    void set_depth_test_enabled(bool enabled)
    {
        if (enabled)
        {
            glEnable(GL_DEPTH_TEST);
        }
        else
        {
            glDisable(GL_DEPTH_TEST);
        }
    }
}
#endif