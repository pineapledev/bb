#include "render_objects.h"

#ifdef NIT_GRAPHICS_API_OPENGL
#include <glad/glad.h>
#endif

namespace Nit
{
    VertexBuffer::VertexBuffer(const void* vertices, u64 size)
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        glCreateBuffers(1, &buffer_id);
        glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
#endif
    }

    VertexBuffer::VertexBuffer(const u64 size)
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        glCreateBuffers(1, &buffer_id);
        glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
#endif
    }

    VertexBuffer::~VertexBuffer()
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        glDeleteBuffers(1, &buffer_id);
#endif
    }

    void VertexBuffer::Bind() const
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
#endif
    }

    void VertexBuffer::Unbind() const
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        glBindBuffer(GL_ARRAY_BUFFER, 0);
#endif
    }

    void VertexBuffer::SetData(const void* data, const u64 size) const
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        glBindBuffer(GL_ARRAY_BUFFER, buffer_id);
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
#endif
    }

    IndexBuffer::IndexBuffer(const u32* indices, u64 count)
        : buffer_id(0)
          , count(count)
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        glCreateBuffers(1, &buffer_id);
        Bind();
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(u32), indices, GL_STATIC_DRAW);
#endif
    }

    IndexBuffer::~IndexBuffer()
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        glDeleteBuffers(1, &buffer_id);
#endif
    }

    void IndexBuffer::Bind() const
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer_id);
#endif
    }

    void IndexBuffer::Unbind() const
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#endif
    }
    
    VertexArray::VertexArray()
        : id(0)
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        glCreateVertexArrays(1, &id);
#endif
    }

    VertexArray::~VertexArray()
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        glDeleteVertexArrays(1, &id);
#endif
    }

    void VertexArray::Bind() const
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        glBindVertexArray(id);
#endif
    }

    void VertexArray::Unbind() const
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        glBindVertexArray(0);
#endif
    }

    void VertexArray::AddVertexBuffer(const TSharedPtr<VertexBuffer>& vertex_buffer)
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        NIT_CHECK_MSG(!vertex_buffer->layout.elements.empty(), "Vertex buffer has no layout!");

        glBindVertexArray(id);
        vertex_buffer->Bind();
        u32 index = 0;
        const BufferLayout& buffer_layout = vertex_buffer->layout;
        for (const auto& element : buffer_layout)
        {
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(
                index,
                GetComponentCountFromShaderDataType(element.type),
                ShaderDataTypeToOpenGL(element.type),
                element.normalized ? GL_TRUE : GL_FALSE,
                buffer_layout.stride,
                reinterpret_cast<void*>(static_cast<intptr_t>(element.offset))
            );
            index++;
        }
        vertex_buffers.push_back(vertex_buffer);
#endif
    }

    void VertexArray::SetIndexBuffer(const TSharedPtr<IndexBuffer>& in_index_buffer)
    {
#ifdef NIT_GRAPHICS_API_OPENGL
        glBindVertexArray(id);
        in_index_buffer->Bind();
        index_buffer = in_index_buffer;
#endif
    }
}