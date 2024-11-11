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
    
    void SetRenderer2DInstance(Renderer2D* renderer_2d_instance)
    {
        NIT_CHECK(renderer_2d_instance);
        renderer_2d = renderer_2d_instance;
    }
    
    void InitRenderer2D(const Renderer2DCfg& cfg)
    {
        NIT_CHECK_RENDERER_2D_CREATED
        
        FinishRenderer2D();
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
            renderer_2d->ibo = CreateIndexBuffer(indices, max_indices);
            delete[] indices;
        }

        // QUAD VO
        {
            constexpr u32 num_of_vertices = MAX_PRIMITIVES * VERTICES_PER_PRIMITIVE;
            renderer_2d->quad_vao = CreateVertexArray();
            renderer_2d->quad_vbo = CreateVertexBuffer(num_of_vertices * sizeof(QuadVertex));
            GetVertexBufferData(renderer_2d->quad_vbo).layout = {
                {ShaderDataType::Float4, "a_Position"},
                {ShaderDataType::Float4, "a_Tint"},
                {ShaderDataType::Float2, "a_UV"},
                {ShaderDataType::Int, "a_Texture"},
                {ShaderDataType::Int, "a_EntityID"}
            };
            AddVertexBuffer(renderer_2d->quad_vao, renderer_2d->quad_vbo);
            renderer_2d->quad_batch = new QuadVertex[num_of_vertices];
            AddIndexBuffer(renderer_2d->quad_vao, renderer_2d->ibo);
        }

        //Texture stuff
        {
            renderer_2d->textures_to_bind.resize(MAX_TEXTURE_SLOTS);

            // White texture
            renderer_2d->white_texture.size       = V2_ONE;
            renderer_2d->white_texture.channels   = 4;
            renderer_2d->white_texture.pixel_data = new u8[]{ 255, 255, 255, 255 };
            LoadTexture2D(&renderer_2d->white_texture);
            
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
            renderer_2d->circle_vao = CreateVertexArray();
            renderer_2d->circle_vbo = CreateVertexBuffer(num_of_vertices * sizeof(CircleVertex));
            GetVertexBufferData(renderer_2d->circle_vbo).layout = {
                {ShaderDataType::Float4, "a_Position"},
                {ShaderDataType::Float4, "a_LocalPosition"},
                {ShaderDataType::Float4, "a_Tint"},
                {ShaderDataType::Float, "a_Thickness"},
                {ShaderDataType::Float, "a_Fade"},
                {ShaderDataType::Int, "a_EntityID"}
            };
            AddVertexBuffer(renderer_2d->circle_vao, renderer_2d->circle_vbo);
            renderer_2d->circle_batch = new CircleVertex[num_of_vertices];
            AddIndexBuffer(renderer_2d->circle_vao, renderer_2d->ibo);
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
            renderer_2d->line_vao = CreateVertexArray();
            renderer_2d->line_vbo = CreateVertexBuffer(num_of_vertices * sizeof(LineVertex));
            GetVertexBufferData(renderer_2d->line_vbo).layout = {
                {ShaderDataType::Float4, "a_Position"},
                {ShaderDataType::Float4, "a_Tint"},
                {ShaderDataType::Int, "a_EntityID"}
            };
            AddVertexBuffer(renderer_2d->line_vao, renderer_2d->line_vbo);
            renderer_2d->line_batch = new LineVertex[num_of_vertices];
            AddIndexBuffer(renderer_2d->line_vao, renderer_2d->ibo);
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
            renderer_2d->char_vao = CreateVertexArray();
            renderer_2d->char_vbo = CreateVertexBuffer(num_of_vertices * sizeof(CharVertex));
            GetVertexBufferData(renderer_2d->char_vbo).layout = {
                {ShaderDataType::Float4, "a_Position"},
                {ShaderDataType::Float4, "a_Tint"},
                {ShaderDataType::Float2, "a_UV"},
                {ShaderDataType::Int, "a_Texture"},
                {ShaderDataType::Int, "a_EntityID"}
            };
            AddVertexBuffer(renderer_2d->char_vao, renderer_2d->char_vbo);
            renderer_2d->char_batch = new CharVertex[num_of_vertices];
            AddIndexBuffer(renderer_2d->char_vao, renderer_2d->ibo);
        }

        // Char Material
        {
            auto shader = CreateSharedPtr<Shader>();
            shader->Compile(text_vertex_shader_source, text_fragment_shader_source);
            renderer_2d->char_material = CreateSharedPtr<Material>(shader);
        }

        NIT_LOG_TRACE("Renderer2D created!");
    }
    
    void StartBatch()
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

    void Flush()
    {
        NIT_CHECK_RENDERER_2D_CREATED
        //SCOPED_PROFILE(Render2D);

        if (const u64 quad_vertex_data_size = (renderer_2d->last_quad_vertex - renderer_2d->quad_batch) * sizeof(QuadVertex))
        {
            NIT_CHECK(renderer_2d->default_material);
            for (u32 i = 0; i < renderer_2d->last_texture_slot; i++)
            {
                BindTexture2D(renderer_2d->textures_to_bind[i], i);
            }

            renderer_2d->default_material->SetConstantSampler2D("u_Textures[0]", &renderer_2d->texture_slots.front(),
                                                               MAX_TEXTURE_SLOTS);
            renderer_2d->default_material->SetConstantMat4("u_ProjectionView", renderer_2d->projection_view);
            renderer_2d->default_material->SubmitConstants();

            SetVertexBufferData(renderer_2d->quad_vbo, renderer_2d->quad_batch, quad_vertex_data_size);
            DrawElements(renderer_2d->quad_vao, renderer_2d->quad_index_count);
        }

        else if (const u64 circle_vertex_data_size = (renderer_2d->last_circle_vertex - renderer_2d->circle_batch) * sizeof(CircleVertex))
        {
            NIT_CHECK(renderer_2d->default_material);
            renderer_2d->default_material->SetConstantMat4("u_ProjectionView", renderer_2d->projection_view);
            renderer_2d->default_material->SubmitConstants();

            SetVertexBufferData(renderer_2d->circle_vbo, renderer_2d->circle_batch, circle_vertex_data_size);
            DrawElements(renderer_2d->circle_vao, renderer_2d->circle_index_count);
        }

        else if (const u64 line_vertex_data_size = (renderer_2d->last_line_vertex - renderer_2d->line_batch) * sizeof(LineVertex))
        {
            NIT_CHECK(renderer_2d->default_material);
            renderer_2d->default_material->SetConstantMat4("u_ProjectionView", renderer_2d->projection_view);
            renderer_2d->default_material->SubmitConstants();

            SetVertexBufferData(renderer_2d->line_vbo, renderer_2d->line_batch, line_vertex_data_size);
            DrawElements(renderer_2d->line_vao, renderer_2d->line_index_count);
        }
        else if (const u64 char_vertex_data_size = (renderer_2d->last_char_vertex - renderer_2d->char_batch) * sizeof(QuadVertex))
        {
            NIT_CHECK(renderer_2d->default_material);
            for (u32 i = 0; i < renderer_2d->last_texture_slot; i++)
            {
                BindTexture2D(renderer_2d->textures_to_bind[i], i);
            }

            renderer_2d->default_material->SetConstantSampler2D("u_Textures[0]", &renderer_2d->texture_slots.front(),
                                                               MAX_TEXTURE_SLOTS);
            renderer_2d->default_material->SetConstantMat4("u_ProjectionView", renderer_2d->projection_view);
            renderer_2d->default_material->SubmitConstants();

            SetVertexBufferData(renderer_2d->char_vbo, renderer_2d->char_batch, char_vertex_data_size);
            DrawElements(renderer_2d->char_vao, renderer_2d->char_index_count);
        }

        SetClearColor(V4_COLOR_DARK_GREY);
    }

    void NextBatch()
    {
        NIT_CHECK_RENDERER_2D_CREATED
        Flush();
        StartBatch();
    }

    void BeginScene2D(const Matrix4& pv_matrix)
    {
        NIT_CHECK_RENDERER_2D_CREATED
        renderer_2d->projection_view = pv_matrix;

        SetBlendingEnabled(true);
        SetBlendingMode(BlendingMode::Alpha);

        StartBatch();
    }

    void PushMaterial2D(const SharedPtr<Material>& material)
    {
        NIT_CHECK_RENDERER_2D_CREATED
        NIT_CHECK(material);
        renderer_2d->custom_material = material;
    }

    void PopMaterial2D()
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
                    NextBatch();
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
            NextBatch();
        }

        renderer_2d->current_shape = shape_to_draw;
        TryUseDefaultMaterial(renderer_2d->current_shape);
    }

    void DrawQuad(
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
            NextBatch();
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

    void DrawCircle(
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
            NextBatch();
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

    void DrawLine2D(
          const V4Verts2D& vertex_positions
        , const V4Verts2D& vertex_colors
        , i32              entity_id
    )
    {
        NIT_CHECK_RENDERER_2D_CREATED
        if (renderer_2d->line_count >= MAX_PRIMITIVES)
        {
            NextBatch();
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

    void DrawChar(
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
            NextBatch();
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

    void DrawText(
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
            
            FillCharVertexData(
                 transform
                , vertex_positions
                , vertex_uvs
                , font
                , size
                ,offset
                , spacing
                , c
            );
            
            FillVertexColors(vertex_colors, tint);
            DrawChar(font, vertex_positions, vertex_uvs, vertex_colors, entity_id);
        }
    }

    void EndScene2D()
    {
        NIT_CHECK_RENDERER_2D_CREATED
        Flush();
    }

    void FinishRenderer2D()
    {
        NIT_CHECK_RENDERER_2D_CREATED

        FreeTexture2D(&renderer_2d->white_texture);
        renderer_2d->white_texture = {};
        delete renderer_2d->quad_batch;
        delete renderer_2d->circle_batch;
        delete renderer_2d->line_batch;
        delete renderer_2d->char_batch;
    }
}