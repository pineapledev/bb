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

    void render_objects_set_instance(RenderObjects* render_objects_instance);
    RenderObjects* get_render_objects_instance();
    void render_objects_init();
        
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
              , size(get_size_of_shader_data_type(type))
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

    VertexBuffer& get_vertex_buffer_data(u32 vertex_buffer);
    void bind_vertex_buffer(u32 vertex_buffer);
    void unbind_vertex_buffer();
    void set_vertex_buffer_data(u32 vertex_buffer, const void* data, u64 size);
    u32  create_vertex_buffer(const void* vertices, u64 size);
    u32  create_vertex_buffer(u64 size);
    void destroy_vertex_buffer(u32 vertex_buffer);
    
    struct IndexBuffer
    {
        u32 buffer_id = 0;
        u64 count = 0;
    };

    void bind_index_buffer(u32 index_buffer);
    void unbind_index_buffer();
    u32  create_index_buffer(const u32* indices, u64 count);
    void destroy_index_buffer(u32 index_buffer);

    struct VertexArray
    {
        u32 id = 0;
    };

    void bind_vertex_array(u32 vertex_array);
    void unbind_vertex_array();
    
    u32  create_vertex_array();
    void destroy_vertex_array(u32 vertex_array);
    void add_index_buffer(u32 vertex_array, u32 index_buffer);
    void add_vertex_buffer(u32 vertex_array, u32 vertex_buffer);
}