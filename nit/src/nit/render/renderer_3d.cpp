#include "renderer_3d.h"
// #include "material.h"
// #include "render_objects.h"
// #include "shader.h"
// #include "shader_sources.h"
//
// namespace Nit
// {
//     struct GPUCube
//     {
//         SharedPtr<VertexArray> vao = nullptr;
//         SharedPtr<VertexBuffer> vbo = nullptr;
//     };
//     
//     void CreateGPUCube(GPUCube& cube)
//     {
//         cube.vao = CreateSharedPtr<VertexArray>();
//
//         static constexpr u32 NUM_OF_VERTICES = 216;
//
//         static FixedArray<f32, NUM_OF_VERTICES> vertices = {
//             -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
//             0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
//             0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
//             0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
//             -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
//             -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
//
//             -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
//             0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
//             0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
//             0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
//             -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
//             -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
//
//             -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
//             -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
//             -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
//             -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
//             -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
//             -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
//
//             0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
//             0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
//             0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
//             0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
//             0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
//             0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
//
//             -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
//             0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
//             0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
//             0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
//             -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
//             -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
//
//             -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
//             0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
//             0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
//             0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
//             -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
//             -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f
//         };
//
//         cube.vbo = CreateSharedPtr<VertexBuffer>(vertices.data(), sizeof(f32) * NUM_OF_VERTICES);
//         cube.vbo->layout = {
//             {ShaderDataType::Float3, "a_LocalPosition"},
//             {ShaderDataType::Float3, "a_Normal"},
//         };
//         cube.vao->AddVertexBuffer(cube.vbo);
//     }
//
//     static constexpr u32 max_cubes = 3000;
//     static constexpr u32 max_light_sources = 100;
//
//     struct Renderer3DContext
//     {
//         Matrix4 projection_view;
//         Transform camera_transform;
//         SharedPtr<Material> default_cube_material = nullptr;
//         LightSource default_ambient_light;
//         Cube* cubes = nullptr;
//         u32 cube_count = 0;
//         LightSource* light_sources = nullptr;
//         u32 light_source_count = 0;
//         GPUCube cube_vertex_objs;
//     };
//
//     Renderer3DContext* context_3d = nullptr;
//
//     void CreateRenderer3D()
//     {
//         NIT_LOG_TRACE("Creating Renderer3D...");
//         NIT_CHECK_MSG(!context_3d, "Renderer already created!");
//         context_3d = new Renderer3DContext();
//
//         SharedPtr<Shader> lit_shader = CreateSharedPtr<Shader>();
//         lit_shader->Compile(lit_vertex_shader_source, lit_fragment_shader_source);
//
//         SharedPtr<MaterialData> mat_data = CreateSharedPtr<MaterialData>();
//         mat_data->ambient_strength = 0.5f;
//         mat_data->diffuse_strength = 0.5f;
//         mat_data->specular_strength = 0.5f;
//         mat_data->color = {0.5f, 0.5f, 0.5f};
//         mat_data->shininess = 1.f;
//
//         context_3d->default_cube_material = CreateSharedPtr<Material>(lit_shader, mat_data);
//         context_3d->cubes = new Cube[max_cubes];
//         context_3d->light_sources = new LightSource[max_light_sources];
//
//         context_3d->light_sources[0] = context_3d->default_ambient_light;
//         context_3d->light_source_count = 1;
//
//         CreateGPUCube(context_3d->cube_vertex_objs);
//     }
//
//     void BeginScene3D(const Matrix4& pv_matrix)
//     {
//         context_3d->projection_view = pv_matrix;
//
//         SetBlendingEnabled(false);
//         SetDepthTestEnabled(true);
//     }
//
//     void DrawCube(const Transform& transform, const SharedPtr<Material>& material /*= nullptr*/)
//     {
//         context_3d->cube_count++;
//         NIT_CHECK_MSG(context_3d->cube_count < max_cubes, "Cube limit reached!");
//
//         Cube& cube = context_3d->cubes[context_3d->cube_count - 1];
//         cube.material = material;
//         cube.transform = transform;
//     }
//
//     void DrawLightSource(const LightSource& source)
//     {
//         context_3d->light_source_count++;
//         NIT_CHECK_MSG(context_3d->light_source_count < max_light_sources, "LightSource limit reached!");
//         context_3d->light_sources[context_3d->light_source_count - 1] = source;
//     }
//
//     void EndScene3D()
//     {
//         //SCOPED_PROFILE(Render3D);
//
//         for (u32 i = 0; i < context_3d->cube_count; i++)
//         {
//             Cube& cube = context_3d->cubes[i];
//             const auto use_material = cube.material ? cube.material : context_3d->default_cube_material;
//             NIT_CHECK(use_material);
//
//             for (u32 j = 0; j < context_3d->light_source_count; j++)
//             {
//                 LightSource& light_source = context_3d->light_sources[j];
//                 Vector3 camera_pos = context_3d->camera_transform.position;
//                 light_source.view_position = { camera_pos.x, camera_pos.y, camera_pos.z };
//             }
//
//             use_material->SetConstantMat4("u_ProjectionView", context_3d->projection_view);
//             use_material->SetConstantMat4("u_Model", ToMatrix4(cube.transform));
//
//             use_material->SetConstantInt("i_NumLights", context_3d->light_source_count);
//
//             use_material->SubmitConstants();
//             use_material->GetShader()->SetConstantObjects("u_Material", 0, 1, sizeof(MaterialData),
//                                                          use_material->GetMaterialData().get());
//             use_material->GetShader()->SetConstantObjects("u_Lights", 1, context_3d->light_source_count,
//                                                          sizeof(LightSource), context_3d->light_sources);
//
//             DrawArrays(context_3d->cube_vertex_objs.vao, 36);
//         }
//
//         context_3d->light_source_count = 1;
//         context_3d->projection_view = Matrix4();
//         context_3d->cube_count = 0;
//     }
//     
//     void DestroyRenderer3D()
//     {
//         NIT_CHECK(context_3d);
//         context_3d->default_cube_material = nullptr;
//         context_3d->cube_vertex_objs = {};
//         context_3d->cube_count = 0;
//         delete[] context_3d->cubes;
//         context_3d->cubes = nullptr;
//         context_3d->light_source_count = 0;
//         delete[] context_3d->light_sources;
//         context_3d->light_sources = nullptr;
//
//         delete context_3d;
//         context_3d = nullptr;
//     }
// }
