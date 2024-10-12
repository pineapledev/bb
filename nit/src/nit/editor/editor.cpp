#include "editor.h"

#include "core/app.h"

#ifdef NIT_EDITOR_ENABLED

#define NIT_CHECK_EDITOR_CREATED NIT_CHECK_MSG(editor, "Forget to call SetEditorInstance!");

namespace Nit
{
    Editor* editor;
    
    void SetEditorInstance(Editor* editor_instance)
    {
        NIT_CHECK(editor_instance);
        editor = editor_instance;
    }

    Editor* GetEditorInstance()
    {
        NIT_CHECK_EDITOR_CREATED
        return editor;
    }

    void InitEditor()
    {
        NIT_CHECK_EDITOR_CREATED
        RetrieveWindowSize((i32*)&editor->frame_buffer.width, (i32*)&editor->frame_buffer.height);

        editor->frame_buffer.color_attachments = {
            FrameBufferTextureFormat::RGBA8,
            FrameBufferTextureFormat::RED_INTEGER
        };

        LoadFrameBuffer(&editor->frame_buffer);
    }

    void BeginDrawEditor()
    {
        NIT_CHECK_EDITOR_CREATED

        if (!editor->enabled)
        {
            app->im_gui_renderer.use_dockspace = false;
            return;
        }

        app->im_gui_renderer.use_dockspace = true;
        
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Windows"))
            {
                ImGui::MenuItem("Viewport", nullptr, &editor->show_viewport);
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        if (editor->show_viewport)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
            ImGui::Begin("Viewport", &editor->show_viewport);
            {
                BindFrameBuffer(&editor->frame_buffer);
                
                // if (!Input::IsEnabled() && ImGui::IsWindowFocused())
                //     Input::SetEnabled(true);
                
                const ImVec2 viewport_offset = ImGui::GetCursorPos();
                const ImVec2 panel_size = ImGui::GetContentRegionAvail();
                
                if (const Vector2 viewport_size = {panel_size.x, panel_size.y}; viewport_size != editor->viewport_size)
                {
                    editor->viewport_size = viewport_size;
                    // set draw system screen size
                    ResizeFrameBuffer(&editor->frame_buffer, (u32)viewport_size.x, (u32)viewport_size.y);
                    BindFrameBuffer(&editor->frame_buffer);
                }
                
                const ImTextureID fb_id = reinterpret_cast<ImTextureID>(editor->frame_buffer.color_attachment_ids[0]);
                ImGui::Image(fb_id, {editor->viewport_size.x, editor->viewport_size.y}, {0, 1}, {1, 0});
                
                ImVec2 window_size = ImGui::GetWindowSize();
                
                ImVec2 min_bound = ImGui::GetWindowPos();
                min_bound.x += viewport_offset.x;
                min_bound.y += viewport_offset.y;
                
                ImVec2 max_bound = {min_bound.x + window_size.x, min_bound.y + window_size.y};
                
                editor->viewport_min_bound = {min_bound.x, min_bound.y};
                editor->viewport_max_bound = {max_bound.x, max_bound.y};
                
                auto [mx, my] = ImGui::GetMousePos();
                mx -= editor->viewport_min_bound.x;
                my -= editor->viewport_min_bound.y;
                my = editor->viewport_size.y - my;
                
                const i32 mouse_x = (i32)mx;
                const i32 mouse_y = (i32)my;
                
                editor->mouse_position = {(f32)mouse_x, (f32)mouse_y};
                
                //Entity selection 
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) /*&& !ImGuizmo::IsOver()*/)
                {
                    if (mouse_x >= 0 && mouse_y >= 0 && mouse_x < (i32)editor->viewport_size.x && mouse_y < (i32)editor
                        ->viewport_size.y)
                    {
                        //i32 entity_id = ReadFrameBufferPixel(&editor->frame_buffer, 1, mouse_x, mouse_y);
                    }
                }

                // Gizmo stuff
                // Entity mainCamera = CameraSystem::GetMainCameraEntity();
                //
                // Entity selectedEntity = EditorSystem::GetSelectedEntity();
                //
                // if (selectedEntity.IsValid() && mainCamera.IsValid())
                // {
                //     ImGuizmo::SetOrthographic(true);
                //     ImGuizmo::SetDrawlist();
                //     ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(),
                //                       ImGui::GetWindowHeight());
                //
                //     static ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
                //     static ImGuizmo::MODE mode = ImGuizmo::WORLD;
                //     static bool bSnapEnabled = false;
                //     static float snap = 0;
                //
                //     if (Input::IsKeyPressed(KeyCode::Key_W)) operation = ImGuizmo::TRANSLATE;
                //     if (Input::IsKeyPressed(KeyCode::Key_E)) operation = ImGuizmo::ROTATE;
                //     if (Input::IsKeyPressed(KeyCode::Key_R)) operation = ImGuizmo::SCALE;
                //
                //     auto& cameraComponent = mainCamera.Get<CameraComponent>();
                //     const float* view = &cameraComponent.ViewMatrix.m[0][0];
                //     const float* projection = &cameraComponent.ProjectionMatrix.m[0][0];
                //
                //     TransformComponent& transform = selectedEntity.GetTransform();
                //     Matrix4 gizmoMatrix = transform.GetMatrix();
                //     float* gizmoMatrixPtr = &gizmoMatrix.m[0][0];
                //
                //     ImGuizmo::Manipulate(view, projection, operation, mode, gizmoMatrixPtr, nullptr,
                //                          bSnapEnabled ? &snap : nullptr);
                //
                //     if (ImGuizmo::IsUsing())
                //     {
                //         float matrixTranslation[3], matrixRotation[3], matrixScale[3];
                //
                //         ImGuizmo::DecomposeMatrixToComponents(gizmoMatrixPtr, matrixTranslation, matrixRotation,
                //                                               matrixScale);
                //         const bool bIsNan = *gizmoMatrixPtr != *gizmoMatrixPtr;
                //         if (!bIsNan)
                //         {
                //             transform.Position = {matrixTranslation[0], matrixTranslation[1], transform.Position.z};
                //             transform.Rotation = {matrixRotation[0], matrixRotation[1], matrixRotation[2]};
                //             transform.Scale = {matrixScale[0], matrixScale[1], matrixScale[2]};
                //         }
                //     }
                //
                //     if (ImGuizmo::IsUsing() && !bWasUsingGizmo)
                //     {
                //         if (selectedEntity.Has<Rigidbody2DComponent>())
                //         {
                //             auto& rb = selectedEntity.Get<Rigidbody2DComponent>();
                //             rb.FollowTransform = true;
                //         }
                //         bWasUsingGizmo = true;
                //     }
                //     else if (!ImGuizmo::IsUsing() && bWasUsingGizmo)
                //     {
                //         if (selectedEntity.Has<Rigidbody2DComponent>())
                //         {
                //             auto& rb = selectedEntity.Get<Rigidbody2DComponent>();
                //             rb.FollowTransform = false;
                //         }
                //         bWasUsingGizmo = false;
                //     }
            }
        }
    }

    void EndDrawEditor()
    {
        if (editor->enabled && editor->show_viewport)
        {
            UnbindFrameBuffer();
            ImGui::End();
            ImGui::PopStyleVar();
        }
    }
}

#endif