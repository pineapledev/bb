#include "render_objects.h"

#ifdef NIT_GRAPHICS_API_OPENGL
#include <glad/glad.h>

#define NIT_CHECK_RENDER_OBJECTS_CREATED NIT_CHECK_MSG(render_objects, "Forget to call SetRenderObjectsInstance!");

namespace Nit
{
    RenderObjects* render_objects = nullptr;
    
    void SetRenderObjectsInstance(RenderObjects* render_objects_instance)
    {
        NIT_CHECK(render_objects_instance);
        render_objects = render_objects_instance;
    }

    RenderObjects* GetRenderObjectsInstance()
    {
        NIT_CHECK_RENDER_OBJECTS_CREATED
        return render_objects;
    }

    void InitRenderObjects()
    {
        NIT_CHECK_RENDER_OBJECTS_CREATED
        Load<VertexArray> (&render_objects->vertex_arrays, 100);
        Load<VertexBuffer>(&render_objects->vertex_buffers, 100);
        Load<IndexBuffer> (&render_objects->index_buffers, 100);
    }

    VertexBuffer& GetVertexBufferData(u32 vertex_buffer)
    {
        return *GetData<VertexBuffer>(&render_objects->vertex_buffers, vertex_buffer);
    }

    void BindVertexBuffer(u32 vertex_buffer)
    {
        auto* vertex_buffer_data = GetData<VertexBuffer>(&render_objects->vertex_buffers, vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_data->buffer_id);
    }

    void UnbindVertexBuffer()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void SetVertexBufferData(u32 vertex_buffer, const void* data, u64 size)
    {
        auto* vertex_buffer_data = GetData<VertexBuffer>(&render_objects->vertex_buffers, vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_data->buffer_id);
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
    }

    u32 CreateVertexBuffer(const void* vertices, u64 size)
    {
        NIT_CHECK_RENDER_OBJECTS_CREATED
        u32 id;
        auto* vertex_buffer_data = InsertData<VertexBuffer>(&render_objects->vertex_buffers, id);
        glCreateBuffers(1, &vertex_buffer_data->buffer_id);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_data->buffer_id);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
        return id;
    }

    u32 CreateVertexBuffer(u64 size)
    {
        NIT_CHECK_RENDER_OBJECTS_CREATED
        u32 id;
        auto* vertex_buffer_data = InsertData<VertexBuffer>(&render_objects->vertex_buffers, id);
        glCreateBuffers(1, &vertex_buffer_data->buffer_id);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_data->buffer_id);
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
        return id;
    }

    void DestroyVertexBuffer(u32 vertex_buffer)
    {
        auto* vertex_buffer_data = GetData<VertexBuffer>(&render_objects->vertex_buffers, vertex_buffer);
        glDeleteBuffers(1, &vertex_buffer_data->buffer_id);
        DeleteData(&render_objects->vertex_buffers, vertex_buffer);
    }

    void BindIndexBuffer(u32 index_buffer)
    {
        auto* index_buffer_data = GetData<IndexBuffer>(&render_objects->index_buffers, index_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data->buffer_id);
    }

    void UnbindIndexBuffer()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    u32 CreateIndexBuffer(const u32* indices, u64 count)
    {
        u32 id;
        auto* index_buffer_data = InsertData<IndexBuffer>(&render_objects->index_buffers, id);
        glCreateBuffers(1, &index_buffer_data->buffer_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data->buffer_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(u32), indices, GL_STATIC_DRAW);
        return id;
    }

    void DestroyIndexBuffer(u32 index_buffer)
    {
        auto* index_buffer_data = GetData<IndexBuffer>(&render_objects->index_buffers, index_buffer);
        glDeleteBuffers(1, &index_buffer_data->buffer_id);
        DeleteData(&render_objects->vertex_buffers, index_buffer);
    }

    void BindVertexArray(u32 vertex_array)
    {
        auto* vertex_array_data = GetData<VertexArray>(&render_objects->vertex_arrays, vertex_array);
        glBindVertexArray(vertex_array_data->id);
    }

    void UnbindVertexArray()
    {
        glBindVertexArray(0);
    }

    u32 CreateVertexArray()
    {
        u32 id;
        auto* vertex_array_data = InsertData<VertexArray>(&render_objects->vertex_arrays, id);
        glCreateVertexArrays(1, &vertex_array_data->id);
        return id;
    }
    
    void DestroyVertexArray(u32 vertex_array)
    {
        auto* vertex_array_data = GetData<VertexArray>(&render_objects->vertex_arrays, vertex_array);
        glDeleteVertexArrays(1, &vertex_array_data->id);
        DeleteData(&render_objects->vertex_arrays, vertex_array);
    }

    void AddIndexBuffer(u32 vertex_array, u32 index_buffer)
    {
        auto* vertex_array_data = GetData<VertexArray>(&render_objects->vertex_arrays, vertex_array);
        glBindVertexArray(vertex_array_data->id);
        auto* index_buffer_data = GetData<IndexBuffer>(&render_objects->index_buffers, index_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data->buffer_id);
    }

    void AddVertexBuffer(u32 vertex_array, u32 vertex_buffer)
    {
        auto* vertex_buffer_data = GetData<VertexBuffer>(&render_objects->vertex_buffers, vertex_buffer);
        
        NIT_CHECK_MSG(!vertex_buffer_data->layout.elements.empty(), "Vertex buffer has no layout!");

        auto* vertex_array_data = GetData<VertexArray>(&render_objects->vertex_arrays, vertex_array);
        glBindVertexArray(vertex_array_data->id);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_data->buffer_id);
        u32 index = 0;
        const BufferLayout& buffer_layout = vertex_buffer_data->layout;
        for (const auto& element : buffer_layout)
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
                        buffer_layout.stride,
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
                        buffer_layout.stride,
                        reinterpret_cast<void*>(static_cast<intptr_t>(element.offset))
                    );
                }
                break;
            }
            index++;
        }
    }
}

#endif