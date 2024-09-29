#pragma once

namespace Nit
{
    struct VertexArray;
    struct VertexBuffer;
    struct IndexBuffer;
    struct Material;
    
    struct MeshTexture
    {
        TString type;
        u32 id;
    };

    struct MeshVertex
    {
        Vector3 position = V3_ZERO;
        Vector3 normal = V3_ZERO;
        Vector2 uv = V2_ZERO;
        i32 entity_id = -1;
    };

    struct Mesh
    {
        Mesh() = default;
        Mesh(const TArray<MeshVertex>& vertices, const TArray<u32>& indices, const TArray<MeshTexture>& textures);

        void UploadToGPU(const TArray<MeshVertex>& in_vertices, const TArray<u32>& in_indices, const TArray<MeshTexture>& in_textures);
        void Draw(const TSharedPtr<Material>& material);

        TArray<MeshVertex> vertices;
        TArray<u32> indices;
        TArray<MeshTexture> textures;

        TSharedPtr<VertexArray> vao;
        TSharedPtr<VertexBuffer> vbo;
        TSharedPtr<IndexBuffer> ibo;
    };    
}