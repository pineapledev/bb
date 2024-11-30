#include "renderer_2d.h"
#include "render_api.h"
#include "render_objects.h"
#include "shader_sources.h"
#include "texture.h"
#include "material.h"
#include "font.h"
#include "shader.h"
#include "primitives_2d.h"

#define NIT_CHECK_RENDERER_2D_CREATED NIT_CHECK_MSG(renderer_2d, "Forget to call SetRenderer2DInstance!");

namespace nit
{
    i32  AssignTextureSlot(Texture2D* texture);
    void TryUseDefaultMaterial(Shape shape);
    void SetCurrentShape(Shape shape_to_draw);
    
    Renderer2D* renderer_2d = nullptr;
    
    void renderer_2d_set_instance(Renderer2D* renderer_2d_instance)
    {
        NIT_CHECK(renderer_2d_instance);
        renderer_2d = renderer_2d_instance;
    }

    Renderer2D* renderer_2d_get_instance()
    {
        NIT_CHECK(renderer_2d);
        return renderer_2d;
    }

    bool renderer_2d_has_instance()
    {
        return renderer_2d != nullptr;
    }

    void renderer_2d_init(const Renderer2DCfg& cfg)
    {
        if (!type_registry_has_instance())
        {
            type_registry_init();
        }
        
        if (!renderer_2d_has_instance())
        {
            static Renderer2D renderer_2d_instance;
            renderer_2d_set_instance(&renderer_2d_instance);
        }

        if (!render_objects_has_instance())
        {
            render_objects_init();
        }
        
        finish_renderer_2d();

        NIT_LOG_TRACE("Creating Renderer2D...");

        // We create the index buffer shared across all the 2D Primitives
        {
            constexpr u32 max_indices = MAX_PRIMITIVES * INDICES_PER_PRIMITIVE;

            const auto indices = new u32[max_indices];

            u32 offset = 0;
            for (u32 i = 0; i < max_indices; i += 6)
            {
                indices[i + 0] = offset + 0;
                indices[i + 1] = offset + 1;
                indices[i + 2] = offset + 2;

                indices[i + 3] = offset + 2;
                indices[i + 4] = offset + 3;
                indices[i + 5] = offset + 0;

                offset += 4;
            }
            renderer_2d->ibo = create_index_buffer(indices, max_indices);
            delete[] indices;
        }

        // QUAD VO
        {
            constexpr u32 num_of_vertices = MAX_PRIMITIVES * VERTICES_PER_PRIMITIVE;
            renderer_2d->quad_vao = create_vertex_array();
            renderer_2d->quad_vbo = create_vertex_buffer(num_of_vertices * sizeof(QuadVertex));
            get_vertex_buffer_data(renderer_2d->quad_vbo).layout = {
                {ShaderDataType::Float4, "a_Position"},
                {ShaderDataType::Float4, "a_Tint"},
                {ShaderDataType::Float2, "a_UV"},
                {ShaderDataType::Int, "a_Texture"},
                {ShaderDataType::Int, "a_EntityID"}
            };
            add_vertex_buffer(renderer_2d->quad_vao, renderer_2d->quad_vbo);
            renderer_2d->quad_batch = new QuadVertex[num_of_vertices];
            add_index_buffer(renderer_2d->quad_vao, renderer_2d->ibo);
        }

        //Texture stuff
        {
            renderer_2d->textures_to_bind.resize(MAX_TEXTURE_SLOTS);

            // White texture
            renderer_2d->white_texture.size       = V2_ONE;
            renderer_2d->white_texture.channels   = 4;
            renderer_2d->white_texture.pixel_data = new u8[]{ 255, 255, 255, 255 };
            texture_2d_load(&renderer_2d->white_texture);
            
            renderer_2d->textures_to_bind[0] = &renderer_2d->white_texture;

            // Texture slots
            renderer_2d->texture_slots.resize(MAX_TEXTURE_SLOTS);
            for (u32 i = 0; i < MAX_TEXTURE_SLOTS; i++)
                renderer_2d->texture_slots[i] = i;
        }

        // QUAD Material
        {
            auto shader = CreateSharedPtr<Shader>();
            shader->Compile(quad_texture_vertex_shader_source, quad_texture_fragment_shader_source);
            renderer_2d->quad_material = CreateSharedPtr<Material>(shader);
        }

        // CIRCLE VO
        {
            constexpr u32 num_of_vertices = MAX_PRIMITIVES * VERTICES_PER_PRIMITIVE;
            renderer_2d->circle_vao = create_vertex_array();
            renderer_2d->circle_vbo = create_vertex_buffer(num_of_vertices * sizeof(CircleVertex));
            get_vertex_buffer_data(renderer_2d->circle_vbo).layout = {
                {ShaderDataType::Float4, "a_Position"},
                {ShaderDataType::Float4, "a_LocalPosition"},
                {ShaderDataType::Float4, "a_Tint"},
                {ShaderDataType::Float, "a_Thickness"},
                {ShaderDataType::Float, "a_Fade"},
                {ShaderDataType::Int, "a_EntityID"}
            };
            add_vertex_buffer(renderer_2d->circle_vao, renderer_2d->circle_vbo);
            renderer_2d->circle_batch = new CircleVertex[num_of_vertices];
            add_index_buffer(renderer_2d->circle_vao, renderer_2d->ibo);
        }

        // Circle Material
        {
            auto shader = CreateSharedPtr<Shader>();
            shader->Compile(circle_vertex_shader_source, circle_fragment_shader_source);
            renderer_2d->circle_material = CreateSharedPtr<Material>(shader);
        }

        // LINE VO
        {
            constexpr u32 num_of_vertices = MAX_PRIMITIVES * VERTICES_PER_PRIMITIVE;
            renderer_2d->line_vao = create_vertex_array();
            renderer_2d->line_vbo = create_vertex_buffer(num_of_vertices * sizeof(LineVertex));
            get_vertex_buffer_data(renderer_2d->line_vbo).layout = {
                {ShaderDataType::Float4, "a_Position"},
                {ShaderDataType::Float4, "a_Tint"},
                {ShaderDataType::Int, "a_EntityID"}
            };
            add_vertex_buffer(renderer_2d->line_vao, renderer_2d->line_vbo);
            renderer_2d->line_batch = new LineVertex[num_of_vertices];
            add_index_buffer(renderer_2d->line_vao, renderer_2d->ibo);
        }

        // Line Material
        {
            auto shader = CreateSharedPtr<Shader>();
            shader->Compile(quad_flat_color_vertex_shader_source, quad_flat_color_fragment_shader_source);
            renderer_2d->line_material = CreateSharedPtr<Material>(shader);
        }

        // CHAR VO
        {
            constexpr u32 num_of_vertices = MAX_PRIMITIVES * VERTICES_PER_PRIMITIVE;
            renderer_2d->char_vao = create_vertex_array();
            renderer_2d->char_vbo = create_vertex_buffer(num_of_vertices * sizeof(CharVertex));
            get_vertex_buffer_data(renderer_2d->char_vbo).layout = {
                {ShaderDataType::Float4, "a_Position"},
                {ShaderDataType::Float4, "a_Tint"},
                {ShaderDataType::Float2, "a_UV"},
                {ShaderDataType::Int, "a_Texture"},
                {ShaderDataType::Int, "a_EntityID"}
            };
            add_vertex_buffer(renderer_2d->char_vao, renderer_2d->char_vbo);
            renderer_2d->char_batch = new CharVertex[num_of_vertices];
            add_index_buffer(renderer_2d->char_vao, renderer_2d->ibo);
        }

        // Char Material
        {
            auto shader = CreateSharedPtr<Shader>();
            shader->Compile(text_vertex_shader_source, text_fragment_shader_source);
            renderer_2d->char_material = CreateSharedPtr<Material>(shader);
        }

        NIT_LOG_TRACE("Renderer2D created!");
    }
    
    void start_batch()
    {
        NIT_CHECK_RENDERER_2D_CREATED
        renderer_2d->last_quad_vertex = renderer_2d->quad_batch;
        renderer_2d->quad_count = 0;
        renderer_2d->last_texture_slot = 1;
        renderer_2d->quad_index_count = 0;

        renderer_2d->last_circle_vertex = renderer_2d->circle_batch;
        renderer_2d->circle_count = 0;
        renderer_2d->circle_index_count = 0;

        renderer_2d->last_line_vertex = renderer_2d->line_batch;
        renderer_2d->line_count = 0;
        renderer_2d->line_index_count = 0;

        renderer_2d->last_char_vertex = renderer_2d->char_batch;
        renderer_2d->char_count = 0;
        renderer_2d->char_index_count = 0;
    }

    void flush()
    {
        NIT_CHECK_RENDERER_2D_CREATED
        //SCOPED_PROFILE(Render2D);

        if (const u64 quad_vertex_data_size = (renderer_2d->last_quad_vertex - renderer_2d->quad_batch) * sizeof(QuadVertex))
        {
            NIT_CHECK(renderer_2d->default_material);
            for (u32 i = 0; i < renderer_2d->last_texture_slot; i++)
            {
                texture_2d_bind(renderer_2d->textures_to_bind[i], i);
            }

            renderer_2d->default_material->SetConstantSampler2D("u_Textures[0]", &renderer_2d->texture_slots.front(),
                                                               MAX_TEXTURE_SLOTS);
            renderer_2d->default_material->SetConstantMat4("u_ProjectionView", renderer_2d->projection_view);
            renderer_2d->default_material->SubmitConstants();

            set_vertex_buffer_data(renderer_2d->quad_vbo, renderer_2d->quad_batch, quad_vertex_data_size);
            draw_elements(renderer_2d->quad_vao, renderer_2d->quad_index_count);
        }

        else if (const u64 circle_vertex_data_size = (renderer_2d->last_circle_vertex - renderer_2d->circle_batch) * sizeof(CircleVertex))
        {
            NIT_CHECK(renderer_2d->default_material);
            renderer_2d->default_material->SetConstantMat4("u_ProjectionView", renderer_2d->projection_view);
            renderer_2d->default_material->SubmitConstants();

            set_vertex_buffer_data(renderer_2d->circle_vbo, renderer_2d->circle_batch, circle_vertex_data_size);
            draw_elements(renderer_2d->circle_vao, renderer_2d->circle_index_count);
        }

        else if (const u64 line_vertex_data_size = (renderer_2d->last_line_vertex - renderer_2d->line_batch) * sizeof(LineVertex))
        {
            NIT_CHECK(renderer_2d->default_material);
            renderer_2d->default_material->SetConstantMat4("u_ProjectionView", renderer_2d->projection_view);
            renderer_2d->default_material->SubmitConstants();

            set_vertex_buffer_data(renderer_2d->line_vbo, renderer_2d->line_batch, line_vertex_data_size);
            draw_elements(renderer_2d->line_vao, renderer_2d->line_index_count);
        }
        else if (const u64 char_vertex_data_size = (renderer_2d->last_char_vertex - renderer_2d->char_batch) * sizeof(QuadVertex))
        {
            NIT_CHECK(renderer_2d->default_material);
            for (u32 i = 0; i < renderer_2d->last_texture_slot; i++)
            {
                texture_2d_bind(renderer_2d->textures_to_bind[i], i);
            }

            renderer_2d->default_material->SetConstantSampler2D("u_Textures[0]", &renderer_2d->texture_slots.front(),
                                                               MAX_TEXTURE_SLOTS);
            renderer_2d->default_material->SetConstantMat4("u_ProjectionView", renderer_2d->projection_view);
            renderer_2d->default_material->SubmitConstants();

            set_vertex_buffer_data(renderer_2d->char_vbo, renderer_2d->char_batch, char_vertex_data_size);
            draw_elements(renderer_2d->char_vao, renderer_2d->char_index_count);
        }
    }

    void next_batch()
    {
        NIT_CHECK_RENDERER_2D_CREATED
        flush();
        start_batch();
    }

    void begin_scene_2d(const Scene2D& scene)
    {
        set_viewport(scene.window_size);
        
        Scene2D& non_const_scene = const_cast<Scene2D&>(scene);
        
        non_const_scene.camera.aspect = scene.window_size.x / scene.window_size.y;   
        
        if (isnan(scene.camera.aspect))
        {
            non_const_scene.camera.aspect = 1280.f / 720.f;
        }

        set_depth_test_enabled(scene.camera.projection == CameraProjection::Perspective);
        begin_scene_2d(camera_proj_view(scene.camera, scene.camera_transform));
    }

    void begin_scene_2d(const Matrix4& pv_matrix)
    {
        NIT_CHECK_RENDERER_2D_CREATED
        renderer_2d->projection_view = pv_matrix;

        set_blending_enabled(true);
        set_blending_mode(BlendingMode::Alpha);

        start_batch();
    }

    void push_material_2d(const SharedPtr<Material>& material)
    {
        NIT_CHECK_RENDERER_2D_CREATED
        NIT_CHECK(material);
        renderer_2d->custom_material = material;
    }

    void pop_material_2d()
    {
        NIT_CHECK_RENDERER_2D_CREATED
        renderer_2d->custom_material = nullptr;
    }

    i32 AssignTextureSlot(Texture2D* texture)
    {
        NIT_CHECK_RENDERER_2D_CREATED
        i32 texture_slot = 0;
        
        if (texture)
        {
            // Search existing texture
            for (u32 i = 1; i < renderer_2d->last_texture_slot; i++)
            {
                if (renderer_2d->textures_to_bind[i]->id == texture->id)
                {
                    texture_slot = i;
                    break;
                }
            }

            // If not exists save the new texture
            if (texture_slot == 0)
            {
                if (renderer_2d->last_texture_slot > MAX_TEXTURE_SLOTS)
                {
                    next_batch();
                }

                renderer_2d->textures_to_bind[renderer_2d->last_texture_slot] = texture;
                texture_slot = renderer_2d->last_texture_slot;
                renderer_2d->last_texture_slot++;
            }
        }
        return texture_slot;
    }

    void TryUseDefaultMaterial(Shape shape)
    {
        NIT_CHECK_RENDERER_2D_CREATED
        if (renderer_2d->custom_material)
        {
            return;
        }

        switch (shape)
        {
        case Shape::Quad:
            renderer_2d->default_material = renderer_2d->quad_material;
            break;
        case Shape::Circle:
            renderer_2d->default_material = renderer_2d->circle_material;
            break;
        case Shape::Line:
            renderer_2d->default_material = renderer_2d->line_material;
            break;
        case Shape::Char:
            renderer_2d->default_material = renderer_2d->char_material;
            break;
        case Shape::None:
        default:
            NIT_CHECK(false);
            break;
        }
    }

    void SetCurrentShape(Shape shape_to_draw)
    {
        NIT_CHECK_RENDERER_2D_CREATED
        if (renderer_2d->current_shape != Shape::None && renderer_2d->current_shape != shape_to_draw)
        {
            TryUseDefaultMaterial(renderer_2d->current_shape);
            next_batch();
        }

        renderer_2d->current_shape = shape_to_draw;
        TryUseDefaultMaterial(renderer_2d->current_shape);
    }

    void draw_quad(
          Texture2D*                  texture_2d
        , const V4Verts2D&            vertex_positions
        , const V2Verts2D&            vertex_uvs      
        , const V4Verts2D&            vertex_colors
        , i32                         entity_id
    )
    {
        NIT_CHECK_RENDERER_2D_CREATED
        if (renderer_2d->quad_count >= MAX_PRIMITIVES)
        {
            next_batch();
        }

        SetCurrentShape(Shape::Quad);

        const i32 texture_slot = AssignTextureSlot(texture_2d);

        for (u8 i = 0; i < VERTICES_PER_PRIMITIVE; ++i)
        {
            QuadVertex& vertex = *renderer_2d->last_quad_vertex;

            vertex.position = vertex_positions[i];
            vertex.uv = vertex_uvs[i];
            vertex.tint = vertex_colors[i];
            vertex.texture = texture_slot;
            vertex.entity_id = entity_id;
            
            renderer_2d->last_quad_vertex++;
        }

        renderer_2d->quad_index_count += INDICES_PER_PRIMITIVE;
        renderer_2d->quad_count++;
    }
    
    void draw_quad(
          const Vector3&  position  
        , const Vector3&  rotation  
        , const Vector3&  scale     
        , const Vector4&  tint      
        , Texture2D*      texture_2d
        , i32             entity_id 
    )
    {
        V4Verts2D vertex_positions = DEFAULT_VERTEX_POSITIONS_2D;
        V2Verts2D vertex_uvs       = DEFAULT_VERTEX_U_VS_2D;
        V4Verts2D vertex_colors    = DEFAULT_VERTEX_COLORS_2D;
        
        if (texture_2d)
        {
            Vector2 size = texture_2d->size;
        
            fill_quad_vertex_u_vs(
                 vertex_uvs
               , false
               , false
               , V2_ONE);
        
            fill_quad_vertex_positions(size , vertex_positions);
        }
        else
        {
            vertex_positions = DEFAULT_VERTEX_POSITIONS_2D;
        }

        transform_vertex_positions(vertex_positions, mat_create_transform(position, rotation, scale));
        fill_vertex_colors(vertex_colors, tint);
        draw_quad(texture_2d, vertex_positions, vertex_uvs, vertex_colors, entity_id);
    }

    void draw_circle(
          const Vector3&              position         
        , const Vector3&              rotation         
        , const Vector3&              scale            
        , const Vector4&              tint             
        , f32                         thickness        
        , f32                         fade             
        , i32                         entity_id        
    )
    {
        V4Verts2D vertex_positions = DEFAULT_VERTEX_POSITIONS_2D;
        V4Verts2D vertex_colors    = DEFAULT_VERTEX_COLORS_2D;

        transform_vertex_positions(vertex_positions, mat_create_transform(position, rotation, scale));
        fill_vertex_colors(vertex_colors, tint);
        draw_circle(vertex_positions, vertex_colors, thickness, fade, entity_id);
    }

    void draw_circle(
          const V4Verts2D& vertex_positions
        , const V4Verts2D& vertex_colors          
        , f32              thickness              
        , f32              fade
        , i32              entity_id
    )
    {
        NIT_CHECK_RENDERER_2D_CREATED
        if (renderer_2d->circle_count >= MAX_PRIMITIVES)
        {
            next_batch();
        }

        SetCurrentShape(Shape::Circle);

        for (u8 i = 0; i < VERTICES_PER_PRIMITIVE; ++i)
        {
            CircleVertex& vertex = *renderer_2d->last_circle_vertex;

            vertex.local_position = DEFAULT_VERTEX_POSITIONS_2D[i];
            vertex.position       = vertex_positions[i];
            vertex.tint           = vertex_colors[i];
            vertex.thickness      = thickness;
            vertex.fade           = fade;
            vertex.entity_id      = entity_id;
            
            renderer_2d->last_circle_vertex++;
        }

        renderer_2d->circle_index_count += INDICES_PER_PRIMITIVE;
        renderer_2d->circle_count++;
    }

    void draw_line_2d(
          const Vector3&              position  
        , const Vector3&              rotation  
        , const Vector3&              scale     
        , const Vector2&              start     
        , const Vector2&              end       
        , const Vector4&              tint      
        , f32                         thickness 
        , i32                         entity_id 
    )
    {
        V4Verts2D vertex_positions = DEFAULT_VERTEX_POSITIONS_2D;
        V4Verts2D vertex_colors    = DEFAULT_VERTEX_COLORS_2D;

        fill_line_2d_vertex_positions(vertex_positions, start, end, thickness);
        transform_vertex_positions(vertex_positions, mat_create_transform(position, rotation, scale));
        fill_vertex_colors(vertex_colors, tint);
        draw_line_2d(vertex_positions, vertex_colors, entity_id);
    }

    void draw_line_2d(
          const V4Verts2D& vertex_positions
        , const V4Verts2D& vertex_colors
        , i32              entity_id
    )
    {
        NIT_CHECK_RENDERER_2D_CREATED
        if (renderer_2d->line_count >= MAX_PRIMITIVES)
        {
            next_batch();
        }

        SetCurrentShape(Shape::Line);

        for (u32 i = 0; i < VERTICES_PER_PRIMITIVE; ++i)
        {
            LineVertex& vertex = *renderer_2d->last_line_vertex;

            vertex.position  = vertex_positions[i];
            vertex.tint      = vertex_colors[i];
            vertex.entity_id = entity_id;

            renderer_2d->last_line_vertex++;
        }

        renderer_2d->line_index_count += INDICES_PER_PRIMITIVE;
        renderer_2d->line_count++;
    }

    void draw_char(
          const Font*      font
        , const V4Verts2D& vertex_positions
        , const V2Verts2D& vertex_uvs
        , const V4Verts2D& vertex_colors
        , i32              entity_id
    )
    {
        NIT_CHECK_RENDERER_2D_CREATED
        if (renderer_2d->char_count >= MAX_PRIMITIVES)
        {
            next_batch();
        }

        SetCurrentShape(Shape::Char);

        const i32 texture_slot = AssignTextureSlot(const_cast<Texture2D*>(&font->atlas));

        for (u8 i = 0; i < VERTICES_PER_PRIMITIVE; ++i)
        {
            CharVertex& vertex = *renderer_2d->last_char_vertex;

            vertex.position  = vertex_positions[i];
            vertex.uv        = vertex_uvs[i];
            vertex.uv        = vertex_uvs[i];
            vertex.tint      = vertex_colors[i];
            vertex.texture   = texture_slot;
            vertex.entity_id = entity_id;
            
            renderer_2d->last_char_vertex++;
        }

        renderer_2d->char_index_count += INDICES_PER_PRIMITIVE;
        renderer_2d->char_count++;
    }

    void draw_text(
          const Font*     font
        , const String&   text
        , const Matrix4&  transform
        , const Vector4&  tint
        , f32             spacing
        , f32             size
        , i32             entity_id
    )
    {
        if (!font)
        {
            return;
        }
        
        Vector2 offset;
        
        for (const char c : text)
        {
            static V4Verts2D vertex_positions = DEFAULT_VERTEX_POSITIONS_2D;
            static V2Verts2D vertex_uvs       = DEFAULT_VERTEX_U_VS_2D;
            static V4Verts2D vertex_colors    = DEFAULT_VERTEX_COLORS_2D;
            
            fill_char_vertex_data(
                 transform
                , vertex_positions
                , vertex_uvs
                , font
                , size
                ,offset
                , spacing
                , c
            );
            
            fill_vertex_colors(vertex_colors, tint);
            draw_char(font, vertex_positions, vertex_uvs, vertex_colors, entity_id);
        }
    }

    void end_scene_2d()
    {
        NIT_CHECK_RENDERER_2D_CREATED
        flush();
    }

    void finish_renderer_2d()
    {
        NIT_CHECK_RENDERER_2D_CREATED

        texture_2d_free(&renderer_2d->white_texture);
        renderer_2d->white_texture = {};
        delete renderer_2d->quad_batch;
        delete renderer_2d->circle_batch;
        delete renderer_2d->line_batch;
        delete renderer_2d->char_batch;
    }
}