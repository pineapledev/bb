#pragma once
#include "render_api.h"

namespace nit
{
    struct RenderObjects
    {
        Pool vertex_arrays;
        Pool vertex_buffers;
        Pool index_buffers;
    };

    void SetRenderObjectsInstance(RenderObjects* render_objects_instance);
    RenderObjects* GetRenderObjectsInstance();
    void InitRenderObjects();
        
    struct BufferElement
    {
        ShaderDataType type = ShaderDataType::None;
        String name;
        bool normalized = false;
        u32 size = 0;
        u32 offset = 0;

        BufferElement() = default;
        
        BufferElement(const ShaderDataType type, const String& name, const bool normalized = false)
            : type(type)
              , name(name)
              , normalized(normalized)
              , size(GetSizeOfShaderDataType(type))
        {
        }
    };

    struct BufferLayout
    {
        BufferLayout() = default;

        BufferLayout(const std::initializer_list<BufferElement>& elements)
            : stride(0)
              , elements(elements)
        {
            u32 offset = 0;
            stride = 0;
            
            for (auto& element : this->elements)
            {
                element.offset = offset;
                offset += element.size;
                stride += element.size;
            }
        }

        Array<BufferElement>::iterator begin() { return elements.begin(); }
        Array<BufferElement>::iterator end() { return elements.end(); }
        Array<BufferElement>::const_iterator begin() const { return elements.begin(); }
        Array<BufferElement>::const_iterator end() const { return elements.end(); }
        
        u32 stride = 0;
        Array<BufferElement> elements;
    };

    struct VertexBuffer
    {
        u32 buffer_id;
        BufferLayout layout;
    };

    VertexBuffer& GetVertexBufferData(u32 vertex_buffer);
    void BindVertexBuffer(u32 vertex_buffer);
    void UnbindVertexBuffer();
    void SetVertexBufferData(u32 vertex_buffer, const void* data, u64 size);
    u32  CreateVertexBuffer(const void* vertices, u64 size);
    u32  CreateVertexBuffer(u64 size);
    void DestroyVertexBuffer(u32 vertex_buffer);
    
    struct IndexBuffer
    {
        u32 buffer_id = 0;
        u64 count = 0;
    };

    void BindIndexBuffer(u32 index_buffer);
    void UnbindIndexBuffer();
    u32  CreateIndexBuffer(const u32* indices, u64 count);
    void DestroyIndexBuffer(u32 index_buffer);

    struct VertexArray
    {
        u32 id = 0;
    };

    void BindVertexArray(u32 vertex_array);
    void UnbindVertexArray();
    
    u32  CreateVertexArray();
    void DestroyVertexArray(u32 vertex_array);
    void AddIndexBuffer(u32 vertex_array, u32 index_buffer);
    void AddVertexBuffer(u32 vertex_array, u32 vertex_buffer);
}