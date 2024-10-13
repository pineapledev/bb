#pragma once
// #include "logic/components.h"
//
// namespace Nit
// {
//     struct Material;
//     
//     struct Cube
//     {
//         Transform transform;
//         SharedPtr<Material> material;
//     };
//
//     enum LightType : u8
//     {
//         LightType_Directional = 0,
//         LightType_Point,
//         LightType_Spot,
//     };
//
//     struct LightSource
//     {
//         Vector4 position = V4_ZERO;
//         Vector4 view_position = V4_ZERO;
//         Vector4 color = V4_ONE;
//         Vector3 direction = V3_ONE;
//
//         f32 specular_strength = 0.f;
//         f32 diffuse_strength = 0.f;
//         f32 ambient_strength = 0.3f;
//
//         f32 constant_att = 0.f;
//         f32 linear_att = 0.f;
//         f32 quad_att = 0.f;
//
//         f32 angle = 0.f;
//         f32 outer_angle = 0.f;
//
//         LightType type = LightType_Directional;
//     };
//     
//     void CreateRenderer3D();
//     void BeginScene3D(Camera& camera, const Transform& camera_transform = {{0.f, 0.f, 3.f}});
//     void BeginScene3D(const Matrix4& pv_matrix = Matrix4());
//     void DrawCube(const Transform& transform, const SharedPtr<Material>& material = nullptr);
//     void DrawLightSource(const LightSource& source = {});
//     void EndScene3D();
//     void DestroyRenderer3D();
// }