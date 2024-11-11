#pragma once

namespace nit
{
    struct Texture2D;
    struct Font;
    
    using V4Verts2D = FixedArray<Vector4, 4>;
    using V3Verts2D = FixedArray<Vector3, 4>;
    using V2Verts2D = FixedArray<Vector2, 4>;
    
    inline constexpr V4Verts2D DEFAULT_VERTEX_POSITIONS_2D = {
        Vector4{-.5f, -.5f, 0.f, 1.f }, // bottom-left
        Vector4{ .5f, -.5f, 0.f, 1.f }, // bottom-right
        Vector4{ .5f,  .5f, 0.f, 1.f }, // top-right
        Vector4{-.5f,  .5f, 0.f, 1.f } // top-left
    };

    inline constexpr V2Verts2D DEFAULT_VERTEX_U_VS_2D = {
        Vector2{0, 0 }, // bottom-left
        Vector2{1, 0 }, // bottom-right
        Vector2{1, 1 }, // top-right
        Vector2{0, 1 } // top-left
    };

    inline constexpr V4Verts2D DEFAULT_VERTEX_COLORS_2D = {
        Vector4{1.f, 1.f, 1.f, 1.f}, // bottom-left
        Vector4{1.f, 1.f, 1.f, 1.f}, // bottom-right
        Vector4{1.f, 1.f, 1.f, 1.f}, // top-right
        Vector4{1.f, 1.f, 1.f, 1.f} // top-left
    };

    struct TextPrimitive
    {
        Matrix4 transform;
        String text;
        Vector4 tint       = V4_ONE;
        f32     spacing    = 1.f;
        f32     size       = 1.f;
    };

    void FillVertexColors(
          V4Verts2D&    vertex_colors
        , const Vector4& color
    );
    
    void TransformVertexPositions(
          V4Verts2D&     vertex_positions
        , const Matrix4& transform
    );

    void FillQuadVertexPositions(
          const Vector2& size
        , V4Verts2D&     vertex_positions
    );
    
    void FillQuadVertexUVs(
          V2Verts2D&     vertex_uvs
        , bool           flip_x
        , bool           flip_y
        , const Vector2& tiling_factor
    );

    void FillQuadVertexUVs(
          V2Verts2D&     vertex_uvs
        , const Vector2& texture_size
        , const Vector2& sub_texture_size
        , const Vector2& sub_texture_location
        , bool           flip_x                = false
        , bool           flip_y                = false
        , const Vector2& tiling_factor         = V2_ONE
    );

    void FillCircleVertexPositions(
          V4Verts2D&  vertex_positions
        , f32         radius
    );
    
    void FillLine2DVertexPositions(
          V4Verts2D&     vertex_positions
        , const Vector2& start
        , const Vector2& end
        , f32            thickness
    );
    
    void FillCharVertexData(
          const Matrix4&          transform
        , V4Verts2D&              vertex_positions
        , V2Verts2D&              vertex_uvs
        , const Font*             font
        , f32                     size
        , Vector2&                offset
        , f32                     spacing
        , char                    c
    );
}