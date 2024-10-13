#include "render_objects.h"

#ifdef NIT_GRAPHICS_API_OPENGL
#include <glad/glad.h>

namespace Nit
{
    VertexBuffer::VertexBuffer(const void* vertices, u64 size)
    {
        glCreateBuffers(1, &buffer_id);
        glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
    }

    VertexBuffer::VertexBuffer(const u64 size)
    {
        glCreateBuffers(1, &buffer_id);
        glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    }

    VertexBuffer::~VertexBuffer()
    {
        glDeleteBuffers(1, &buffer_id);
    }

    void VertexBuffer::Bind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
    }

    void VertexBuffer::Unbind() const
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void VertexBuffer::SetData(const void* data, const u64 size) const
    {
        glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
    }

    IndexBuffer::IndexBuffer(const u32* indices, u64 count)
        : buffer_id(0)
          , count(count)
    {
        glCreateBuffers(1, &buffer_id);
        Bind();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(u32), indices, GL_STATIC_DRAW);
    }

    IndexBuffer::~IndexBuffer()
    {
        glDeleteBuffers(1, &buffer_id);
    }

    void IndexBuffer::Bind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_id);
    }

    void IndexBuffer::Unbind() const
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    
    VertexArray::VertexArray()
        : id(0)
    {
        glCreateVertexArrays(1, &id);
    }

    VertexArray::~VertexArray()
    {
        glDeleteVertexArrays(1, &id);
    }

    void VertexArray::Bind() const
    {
        glBindVertexArray(id);
    }

    void VertexArray::Unbind() const
    {
        glBindVertexArray(0);
    }

    void EnableVertexAttribute(const BufferElement& element, u32 index, u32 stride)
    {
        glEnableVertexAttribArray(index);
        switch (element.type)
        {
        case ShaderDataType::None:
        case ShaderDataType::Float:
        case ShaderDataType::Float2:
        case ShaderDataType::Float3:
        case ShaderDataType::Float4:
        case ShaderDataType::Mat3:
        case ShaderDataType::Mat4:
        case ShaderDataType::Sampler2D:
            {
                glVertexAttribPointer(
                    index,
                    GetComponentCountFromShaderDataType(element.type),
                    ShaderDataTypeToOpenGL(element.type),
                    element.normalized ? GL_TRUE : GL_FALSE,
                    stride,
                    reinterpret_cast<void*>(static_cast<intptr_t>(element.offset))
                );
            }
            break;
        case ShaderDataType::Int:
        case ShaderDataType::Int2:
        case ShaderDataType::Int3:
        case ShaderDataType::Int4:
        case ShaderDataType::Bool:
            {
                glVertexAttribIPointer(
                    index,
                    GetComponentCountFromShaderDataType(element.type),
                    ShaderDataTypeToOpenGL(element.type),
                    stride,
                    reinterpret_cast<void*>(static_cast<intptr_t>(element.offset))
                );
            }
            break;
        }
    }
    
    void VertexArray::AddVertexBuffer(const SharedPtr<VertexBuffer>& vertex_buffer)
    {
        NIT_CHECK_MSG(!vertex_buffer->layout.elements.empty(), "Vertex buffer has no layout!");

        glBindVertexArray(id);
        vertex_buffer->Bind();
        u32 index = 0;
        const BufferLayout& buffer_layout = vertex_buffer->layout;
        for (const auto& element : buffer_layout)
        {
            EnableVertexAttribute(element, index, buffer_layout.stride);
            index++;
        }
        vertex_buffers.push_back(vertex_buffer);
    }

    void VertexArray::SetIndexBuffer(const SharedPtr<IndexBuffer>& in_index_buffer)
    {
        glBindVertexArray(id);
        in_index_buffer->Bind();
        index_buffer = in_index_buffer;
    }
}

#endif