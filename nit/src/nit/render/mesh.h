#pragma once

namespace Nit
{
    struct VertexArray;
    struct VertexBuffer;
    struct IndexBuffer;
    struct Material;
    
    struct MeshTexture
    {
        String type;
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
        Mesh(const Array<MeshVertex>& vertices, const Array<u32>& indices, const Array<MeshTexture>& textures);

        void UploadToGPU(const Array<MeshVertex>& in_vertices, const Array<u32>& in_indices, const Array<MeshTexture>& in_textures);
        void Draw(const SharedPtr<Material>& material);

        Array<MeshVertex> vertices;
        Array<u32> indices;
        Array<MeshTexture> textures;

        SharedPtr<VertexArray> vao;
        SharedPtr<VertexBuffer> vbo;
        SharedPtr<IndexBuffer> ibo;
    };    
}