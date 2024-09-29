#pragma once
#include "render_api.h"

namespace Nit
{
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
        VertexBuffer(const void* vertices, u64 size);
        VertexBuffer(u64 size);
        ~VertexBuffer();

        void Bind() const;
        void Unbind() const;
        void SetData(const void* data, u64 size) const;

        u32 buffer_id;
        BufferLayout layout;
    };

    struct IndexBuffer
    {
        IndexBuffer(const u32* indices, u64 count);
        ~IndexBuffer();

        void Bind() const;
        void Unbind() const;

        u32 buffer_id;
        u64 count;
    };

    struct VertexArray
    {
        VertexArray();
        ~VertexArray();

        void Bind() const;
        void Unbind() const;

        void AddVertexBuffer(const SharedPtr<VertexBuffer>& vertex_buffer);
        void SetIndexBuffer(const SharedPtr<IndexBuffer>& in_index_buffer);

        u32 id;
        Array<SharedPtr<VertexBuffer>> vertex_buffers;
        SharedPtr<IndexBuffer> index_buffer;
    };
}