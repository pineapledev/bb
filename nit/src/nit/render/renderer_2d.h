#pragma once
#include "primitives_2d.h"
#include "texture.h"

namespace nit
{
    inline static constexpr u32 MAX_PRIMITIVES = 100000;
    inline static constexpr u32 VERTICES_PER_PRIMITIVE = 4;
    inline static constexpr u32 INDICES_PER_PRIMITIVE = 6;
    inline static constexpr u32 MAX_TEXTURE_SLOTS = 32;
    
    struct VertexArray;
    struct VertexBuffer;
    struct IndexBuffer;
    struct Material;
    struct Texture2D;
    struct Font;
    
    struct Renderer2DCfg
    {
    };

    enum class Shape : u8
    {
        None, Quad, Circle, Line, Char
    };

    struct QuadVertex
    {
        Vector4 position  = V4_ZERO;
        Vector4 tint      = V4_ONE;
        Vector2 uv        = V2_ZERO;
        i32     texture   = 0;
        i32     entity_id = -1;
    };

    struct CircleVertex
    {
        Vector4 position       = V4_ZERO;
        Vector4 local_position = V4_ZERO;
        Vector4 tint           = V4_ONE;
        f32     thickness      = .05f;
        f32     fade           = .01f;
        i32     entity_id      = -1;
    };

    struct LineVertex
    {
        Vector4 position = V4_ZERO;
        Vector4 tint     = V4_ONE;
        i32 entity_id    = -1;
    };

    struct CharVertex
    {
        Vector4 position = V4_ZERO;
        Vector4 tint     = V4_ONE;
        Vector2 uv       = V2_ZERO;
        i32 texture      = 0;
        i32 entity_id    = -1;
    };

    struct Scene2D
    {
        
    };

    void        renderer_2d_set_instance(struct Renderer2D* renderer_2d_instance);
    Renderer2D* renderer_2d_get_instance();
    bool        renderer_2d_has_instance();
    void        renderer_2d_init(const Renderer2DCfg& cfg = {});
    
    void start_batch();
    
    void flush();
    
    void next_batch();
    
    void begin_scene_2d(const Matrix4& pv_matrix);
    
    void push_material_2d(const SharedPtr<Material>& material);
    
    void pop_material_2d();
    
    void draw_quad(
          Texture2D*                  texture_2d       = nullptr
        , const V4Verts2D&            vertex_positions = DEFAULT_VERTEX_POSITIONS_2D
        , const V2Verts2D&            vertex_uvs       = DEFAULT_VERTEX_U_VS_2D
        , const V4Verts2D&            vertex_colors    = DEFAULT_VERTEX_COLORS_2D
        , i32                         entity_id        = -1
    );
    
    void draw_quad(
          const Vector3&              position         = V3_ZERO
        , const Vector3&              rotation         = V3_ZERO
        , const Vector3&              scale            = V3_ONE
        , const Vector4&              tint             = V4_ONE
        , Texture2D*                  texture_2d       = nullptr
        , i32                         entity_id        = -1
    );
    
    void draw_circle(                                    
          const V4Verts2D&            vertex_positions  = DEFAULT_VERTEX_POSITIONS_2D
        , const V4Verts2D&            vertex_colors     = DEFAULT_VERTEX_COLORS_2D
        , f32                         thickness         = .05f
        , f32                         fade              = .01f
        , i32                         entity_id         = -1
    );                                                  
                                                        
    void draw_line_2d(                                    
          const V4Verts2D&            vertex_positions  = DEFAULT_VERTEX_POSITIONS_2D
        , const V4Verts2D&            vertex_colors     = DEFAULT_VERTEX_COLORS_2D
        , i32                         entity_id         = -1
    );                                                  
                                                        
    void draw_char(                                      
          const Font*                 font             = nullptr
        , const V4Verts2D&            vertex_positions = DEFAULT_VERTEX_POSITIONS_2D
        , const V2Verts2D&            vertex_uvs       = DEFAULT_VERTEX_U_VS_2D
        , const V4Verts2D&            vertex_colors    = DEFAULT_VERTEX_COLORS_2D
        , i32                         entity_id        = -1
    );
    
    void draw_text(
          const Font*            font
        , const String&          text
        , const Matrix4&         transform  = {}
        , const Vector4&         tint       = V4_ONE
        , f32                    spacing    = 1.f
        , f32                    size       = 1.f
        , i32                    entity_id  = -1
    );
    
    void end_scene_2d();
    
    void finish_renderer_2d();

    struct Renderer2D
    {
        Matrix4             projection_view;   
        Array<Texture2D*>   textures_to_bind   = {};
        Array<i32>          texture_slots      = {};
        u32                 last_texture_slot  = 1;
        SharedPtr<Material> default_material   = nullptr;
        SharedPtr<Material> custom_material    = nullptr;
        u32                 ibo                = 0;
        Shape               current_shape      = Shape::None;
        u32                 quad_vao           = 0;
        u32                 quad_vbo           = 0;
        QuadVertex*         quad_batch         = nullptr;
        QuadVertex*         last_quad_vertex   = nullptr;
        SharedPtr<Material> quad_material      = nullptr;
        Texture2D           white_texture;
        u32                 quad_count         = 0;
        u32                 quad_index_count   = 0;
        u32                 circle_vao         = 0;
        u32                 circle_vbo         = 0;
        CircleVertex*       circle_batch       = nullptr;
        CircleVertex*       last_circle_vertex = nullptr;
        SharedPtr<Material> circle_material    = nullptr;
        u32                 circle_count       = 0;
        u32                 circle_index_count = 0;
        u32                 line_vao           = 0;
        u32                 line_vbo           = 0;
        LineVertex*         line_batch         = nullptr;
        LineVertex*         last_line_vertex   = nullptr;
        SharedPtr<Material> line_material      = nullptr;
        u32                 line_count         = 0;
        u32                 line_index_count   = 0;
        u32                 char_vao           = 0;
        u32                 char_vbo           = 0;
        CharVertex*         char_batch         = nullptr;
        CharVertex*         last_char_vertex   = nullptr;
        SharedPtr<Material> char_material      = nullptr;
        u32                 char_count         = 0;
        u32                 char_index_count   = 0;
    };
}