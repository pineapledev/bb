#pragma once

namespace nit
{
    struct VertexArray;
    struct VertexBuffer;
    
    enum class BlendingMode : u8
    {
        Alpha,
        Solid,
        Add,
        Multiply
    };

    enum class ShaderDataType : u8
    {
        None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Sampler2D, Int2, Int3, Int4, Bool
    };
    
    ShaderDataType shader_data_type_from_open_gl(i32 type);
    i32            shader_data_type_to_open_gl(ShaderDataType type);
    u32            get_size_of_shader_data_type(ShaderDataType type);
    u32            get_component_count_from_shader_data_type(ShaderDataType type);
    void*          create_from_shader_data_type(ShaderDataType type);
    
    void  set_viewport(u32 x, u32 y, u32 width, u32 height);
    void  set_clear_color(const Vector4& clear_color);
    void  clear_screen();
    void  set_blending_enabled(bool enabled);
    void  set_blending_mode(BlendingMode blending_mode);
    void  draw_elements(u32 vao, u32 element_count);
    void  draw_arrays(u32 vao, u32 element_count);
    void  set_depth_test_enabled(bool enabled);
}