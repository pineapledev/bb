#include "mesh.h"
// #include "render_api.h"
// #include "render_objects.h"
// #include "material.h"
//
// #ifdef NIT_GRAPHICS_API_OPENGL
// #include <glad/glad.h>
// #endif
//
//
// namespace Nit
// {
//     Mesh::Mesh(const Array<MeshVertex>& vertices, const Array<u32>& indices, const Array<MeshTexture>& textures)
//     {
//         UploadToGPU(vertices, indices, textures);
//     }
//
//     void Mesh::UploadToGPU(const Array<MeshVertex>& in_vertices, const Array<u32>& in_indices,
//                            const Array<MeshTexture>& in_textures)
//     {
//         vertices = in_vertices;
//         indices = in_indices;
//         textures = in_textures;
//
//         vao = CreateSharedPtr<VertexArray>();
//         vbo = CreateSharedPtr<VertexBuffer>(in_vertices.data(), sizeof(MeshVertex) * in_vertices.size());
//
//         vbo->layout = {
//             {ShaderDataType::Float3, "a_LocalPosition"},
//             {ShaderDataType::Float3, "a_Normal"},
//             {ShaderDataType::Float2, "a_UV"}
//         };
//
//         vao->AddVertexBuffer(vbo);
//         ibo = CreateSharedPtr<IndexBuffer>(in_indices.data(), in_indices.size());
//         vao->SetIndexBuffer(ibo);
//     }
//
//     void Mesh::Draw(const SharedPtr<Material>& material)
//     {
//         u32 diffuse_nr = 1;
//         u32 specular_nr = 1;
//
//         for (i32 i = 0; i < textures.size(); i++)
//         {
//             glActiveTexture(GL_TEXTURE0 + i);
//             String number;
//             String name = textures[i].type;
//
//             if (name == "texture_diffuse")
//             {
//                 number = std::to_string(diffuse_nr++);
//             }
//             else if (name == "texture_specular")
//             {
//                 number = std::to_string(specular_nr++);
//             }
//
//             const String constant_name = String("material.").append(name).append(number);
//             material->SetConstantInt(constant_name, i);
//             glBindTexture(GL_TEXTURE_2D, textures[i].id);
//         }
//         glActiveTexture(GL_TEXTURE0);
//
//         DrawElements(vao, (u32) ibo->count);
//     }
// }
