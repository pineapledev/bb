#pragma once

namespace Nit
{
    inline static constexpr u32 MAX_PRIMITIVES = 3000;
    inline static constexpr u32 VERTICES_PER_PRIMITIVE = 4;
    inline static constexpr u32 INDICES_PER_PRIMITIVE = 6;
    inline static constexpr u32 MAX_TEXTURE_SLOTS = 32;
    
    struct VertexArray;
    struct VertexBuffer;
    struct IndexBuffer;
    struct Material;
    struct Texture2D;
    struct Font;
    struct QuadVertexData;
    struct CircleVertexData;
    struct Line2DVertexData;
    struct CharVertexData;
    
    struct Renderer2DCfg
    {
    };

    struct Quad
    {
        Matrix4 transform;
        Vector4 tint = V4_ONE;
        Vector2 size = V2_ONE;
        bool flip_x = false;
        bool flip_y = false;
        Vector2 tiling_factor = V2_ONE;
        bool keep_aspect = true;
    };

    struct Circle
    {
        Matrix4 transform;
        Vector4 tint = V4_ONE;
        f32 radius = .5f;
        f32 thickness = .05f;
        f32 fade = .01f;
    };

    struct Line2D
    {
        Matrix4 transform;
        Vector4 tint = V4_ONE;
        Vector2 start = V2_ZERO;
        Vector2 end = V2_ONE;
        f32 thickness = .05f;
    };

    struct Char
    {
        Matrix4 transform;
        char c = ' ';
        TSharedPtr<Font> font = nullptr;
        Vector4 tint = V4_ONE;
        f32 spacing = 1.f;
        f32 size = 1.f;
    };

    struct Text
    {
        Matrix4 transform;
        TString text;
        TSharedPtr<Font> font = nullptr;
        Vector4 tint = V4_ONE;
        f32 spacing = 1.f;
        f32 size = 1.f;
    };

    enum class Shape : u8
    {
        None, Quad, Circle, Line, Char
    };

    struct QuadVertex
    {
        Vector4 position = V4_ZERO;
        Vector4 tint = V4_ONE;
        Vector2 uv = V2_ZERO;
        i32 texture = 0;
        i32 entity_id = -1;
    };

    struct CircleVertex
    {
        Vector4 position = V4_ZERO;
        Vector4 local_position = V4_ZERO;
        Vector4 tint = V4_ONE;
        f32 thickness = .05f;
        f32 fade = .01f;
        i32 entity_id = -1;
    };

    struct LineVertex
    {
        Vector4 position = V4_ZERO;
        Vector4 tint = V4_ONE;
        i32 entity_id = -1;
    };

    struct CharVertex
    {
        Vector4 position = V4_ZERO;
        Vector4 tint = V4_ONE;
        Vector2 uv = V2_ZERO;
        i32 texture = 0;
        i32 entity_id = -1;
    };

    void SetRenderer2DInstance(struct Renderer2D* renderer_2d_instance);
    void InitRenderer2D(const Renderer2DCfg& cfg = {});
    void StartBatch();
    void Flush();
    void NextBatch();
    void BeginScene2D(const Matrix4& pv_matrix);
    void PushMaterial2D(const TSharedPtr<Material>& material);
    void PopMaterial2D();
    void DrawQuad(const QuadVertexData& quad, const TSharedPtr<Texture2D>& texture_2d = nullptr);
    void DrawQuad(const Quad& quad = {}, const TSharedPtr<Texture2D>& texture_2d = nullptr);
    void DrawCircle(const CircleVertexData& circle);
    void DrawCircle(const Circle& circle = {});
    void DrawLine2D(const Line2DVertexData& line);
    void DrawLine2D(const Line2D& line = {});
    void DrawChar(const CharVertexData& character);
    void DrawChar(const Char& character, Vector2& offset);
    void DrawText(const Text& text = {});
    void EndScene2D();
    void FinishRenderer2D();

    struct Renderer2D
    {
        TArray<TSharedPtr<Texture2D>> textures_to_bind;
        TArray<i32> texture_slots;
        u32 last_texture_slot = 1;
        Matrix4 projection_view;
        TSharedPtr<Material> default_material;
        TSharedPtr<Material> custom_material;
        TSharedPtr<IndexBuffer> ibo;
        Shape current_shape = Shape::None;

        // Quad stuff                                              
        TSharedPtr<VertexArray> quad_vao;
        TSharedPtr<VertexBuffer> quad_vbo;
        QuadVertex* quad_batch = nullptr;
        QuadVertex* last_quad_vertex = nullptr;
        TSharedPtr<Material> quad_material;
        TSharedPtr<Texture2D> white_texture;
        u32 quad_count = 0;
        u32 quad_index_count = 0;
        
        // Circle stuff
        TSharedPtr<VertexArray> circle_vao;
        TSharedPtr<VertexBuffer> circle_vbo;
        CircleVertex* circle_batch = nullptr;
        CircleVertex* last_circle_vertex = nullptr;
        TSharedPtr<Material> circle_material;
        u32 circle_count = 0;
        u32 circle_index_count = 0;

        // Line stuff
        TSharedPtr<VertexArray> line_vao;
        TSharedPtr<VertexBuffer> line_vbo;
        LineVertex* line_batch = nullptr;
        LineVertex* last_line_vertex = nullptr;
        TSharedPtr<Material> line_material;
        u32 line_count = 0;
        u32 line_index_count = 0;

        // Char stuff
        TSharedPtr<VertexArray> char_vao;
        TSharedPtr<VertexBuffer> char_vbo;
        CharVertex* char_batch = nullptr;
        CharVertex* last_char_vertex = nullptr;
        TSharedPtr<Material> char_material;
        u32 char_count = 0;
        u32 char_index_count = 0;
    };
}