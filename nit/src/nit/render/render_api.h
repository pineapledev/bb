#pragma once

namespace Nit
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
    
    ShaderDataType ShaderDataTypeFromOpenGL(i32 type);
    i32            ShaderDataTypeToOpenGL(ShaderDataType type);
    u32            GetSizeOfShaderDataType(ShaderDataType type);
    u32            GetComponentCountFromShaderDataType(ShaderDataType type);
    void*          CreateFromShaderDataType(ShaderDataType type);
    
    void  SetViewport(u32 x, u32 y, u32 width, u32 height);
    void  SetClearColor(const Vector4& clear_color);
    void  ClearScreen();
    void  SetBlendingEnabled(bool enabled);
    void  SetBlendingMode(BlendingMode blending_mode);
    void  DrawElements(const TSharedPtr<VertexArray>& vao, u32 element_count);
    void  DrawArrays(const TSharedPtr<VertexArray>& vao, u32 element_count);
    void  SetDepthTestEnabled(bool enabled);
}