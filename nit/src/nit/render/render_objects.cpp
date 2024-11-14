#include "render_objects.h"

#ifdef NIT_GRAPHICS_API_OPENGL
#include <glad/glad.h>

#define NIT_CHECK_RENDER_OBJECTS_CREATED NIT_CHECK_MSG(render_objects, "Forget to call SetRenderObjectsInstance!");

namespace nit
{
    RenderObjects* render_objects = nullptr;
    
    void render_objects_set_instance(RenderObjects* render_objects_instance)
    {
        NIT_CHECK(render_objects_instance);
        render_objects = render_objects_instance;
    }

    RenderObjects* get_render_objects_instance()
    {
        NIT_CHECK_RENDER_OBJECTS_CREATED
        return render_objects;
    }

    void render_objects_init()
    {
        NIT_CHECK_RENDER_OBJECTS_CREATED
        pool_load<VertexArray> (&render_objects->vertex_arrays, 100);
        pool_load<VertexBuffer>(&render_objects->vertex_buffers, 100);
        pool_load<IndexBuffer> (&render_objects->index_buffers, 100);
    }

    VertexBuffer& get_vertex_buffer_data(u32 vertex_buffer)
    {
        return *pool_get_data<VertexBuffer>(&render_objects->vertex_buffers, vertex_buffer);
    }

    void bind_vertex_buffer(u32 vertex_buffer)
    {
        auto* vertex_buffer_data = pool_get_data<VertexBuffer>(&render_objects->vertex_buffers, vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_data->buffer_id);
    }

    void unbind_vertex_buffer()
    {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void set_vertex_buffer_data(u32 vertex_buffer, const void* data, u64 size)
    {
        auto* vertex_buffer_data = pool_get_data<VertexBuffer>(&render_objects->vertex_buffers, vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_data->buffer_id);
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
    }

    u32 create_vertex_buffer(const void* vertices, u64 size)
    {
        NIT_CHECK_RENDER_OBJECTS_CREATED
        u32 id;
        auto* vertex_buffer_data = pool_insert_data<VertexBuffer>(&render_objects->vertex_buffers, id);
        glCreateBuffers(1, &vertex_buffer_data->buffer_id);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_data->buffer_id);
        glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
        return id;
    }

    u32 create_vertex_buffer(u64 size)
    {
        NIT_CHECK_RENDER_OBJECTS_CREATED
        u32 id;
        auto* vertex_buffer_data = pool_insert_data<VertexBuffer>(&render_objects->vertex_buffers, id);
        glCreateBuffers(1, &vertex_buffer_data->buffer_id);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_data->buffer_id);
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
        return id;
    }

    void destroy_vertex_buffer(u32 vertex_buffer)
    {
        auto* vertex_buffer_data = pool_get_data<VertexBuffer>(&render_objects->vertex_buffers, vertex_buffer);
        glDeleteBuffers(1, &vertex_buffer_data->buffer_id);
        pool_delete_data(&render_objects->vertex_buffers, vertex_buffer);
    }

    void bind_index_buffer(u32 index_buffer)
    {
        auto* index_buffer_data = pool_get_data<IndexBuffer>(&render_objects->index_buffers, index_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data->buffer_id);
    }

    void unbind_index_buffer()
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    u32 create_index_buffer(const u32* indices, u64 count)
    {
        u32 id;
        auto* index_buffer_data = pool_insert_data<IndexBuffer>(&render_objects->index_buffers, id);
        glCreateBuffers(1, &index_buffer_data->buffer_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data->buffer_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(u32), indices, GL_STATIC_DRAW);
        return id;
    }

    void destroy_index_buffer(u32 index_buffer)
    {
        auto* index_buffer_data = pool_get_data<IndexBuffer>(&render_objects->index_buffers, index_buffer);
        glDeleteBuffers(1, &index_buffer_data->buffer_id);
        pool_delete_data(&render_objects->vertex_buffers, index_buffer);
    }

    void bind_vertex_array(u32 vertex_array)
    {
        auto* vertex_array_data = pool_get_data<VertexArray>(&render_objects->vertex_arrays, vertex_array);
        glBindVertexArray(vertex_array_data->id);
    }

    void unbind_vertex_array()
    {
        glBindVertexArray(0);
    }

    u32 create_vertex_array()
    {
        u32 id;
        auto* vertex_array_data = pool_insert_data<VertexArray>(&render_objects->vertex_arrays, id);
        glCreateVertexArrays(1, &vertex_array_data->id);
        return id;
    }
    
    void destroy_vertex_array(u32 vertex_array)
    {
        auto* vertex_array_data = pool_get_data<VertexArray>(&render_objects->vertex_arrays, vertex_array);
        glDeleteVertexArrays(1, &vertex_array_data->id);
        pool_delete_data(&render_objects->vertex_arrays, vertex_array);
    }

    void add_index_buffer(u32 vertex_array, u32 index_buffer)
    {
        auto* vertex_array_data = pool_get_data<VertexArray>(&render_objects->vertex_arrays, vertex_array);
        glBindVertexArray(vertex_array_data->id);
        auto* index_buffer_data = pool_get_data<IndexBuffer>(&render_objects->index_buffers, index_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_data->buffer_id);
    }

    void add_vertex_buffer(u32 vertex_array, u32 vertex_buffer)
    {
        auto* vertex_buffer_data = pool_get_data<VertexBuffer>(&render_objects->vertex_buffers, vertex_buffer);
        
        NIT_CHECK_MSG(!vertex_buffer_data->layout.elements.empty(), "Vertex buffer has no layout!");

        auto* vertex_array_data = pool_get_data<VertexArray>(&render_objects->vertex_arrays, vertex_array);
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
                        get_component_count_from_shader_data_type(element.type),
                        shader_data_type_to_open_gl(element.type),
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
                        get_component_count_from_shader_data_type(element.type),
                        shader_data_type_to_open_gl(element.type),
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