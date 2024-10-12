#include "editor.h"

#include "core/app.h"
#include "logic/components.h"

#ifdef NIT_EDITOR_ENABLED
#include <ImGuizmo.h>

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
            return;
        }

        if (app->im_gui_renderer.is_dockspace_enabled && ImGui::BeginMenuBar())
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
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGuizmo::IsOver())
                {
                    if (mouse_x >= 0 && mouse_y >= 0 && mouse_x < (i32)editor->viewport_size.x && mouse_y < (i32)editor
                        ->viewport_size.y)
                    {
                        i32 entity_id = ReadFrameBufferPixel(&editor->frame_buffer, 1, mouse_x, mouse_y);
                        if ((editor->is_entity_selected = entity_id >= 0))
                        {
                            editor->selected_entity = (Entity) entity_id;
                        }
                    }
                }

                auto& camera_group = GetEntityGroup<Camera, Transform>();

                if (!camera_group.entities.empty())
                {
                    Entity camera_entity = *camera_group.entities.begin();

                    // Gizmo stuff
                    Entity selected_entity = editor->selected_entity;
                    
                    if (editor->is_entity_selected && IsEntityValid(selected_entity) && IsEntityValid(camera_entity))
                    {
                        auto& camera_data      = GetComponent<Camera>(camera_entity);
                        auto& camera_transform = GetComponent<Transform>(camera_entity);
                        
                        ImGuizmo::SetOrthographic(camera_data.projection == CameraProjection::Orthographic);
                        ImGuizmo::SetDrawlist();
                        const auto& window_pos = ImGui::GetWindowPos();
                        ImGuizmo::SetRect(window_pos.x, window_pos.y, window_size.x, window_size.y);
                        
                        static ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
                        static ImGuizmo::MODE mode = ImGuizmo::LOCAL;
                        static bool snap_enabled = false;
                        static float snap = 0;
                        
                        if (ImGui::IsKeyPressed(ImGuiKey_W)) operation = ImGuizmo::TRANSLATE;
                        if (ImGui::IsKeyPressed(ImGuiKey_E)) operation = ImGuizmo::ROTATE;
                        if (ImGui::IsKeyPressed(ImGuiKey_R)) operation = ImGuizmo::SCALE;

                        const float* view = &CalculateViewMatrix(camera_transform).m[0][0];
                        const float* projection = &CalculateProjectionMatrix(camera_data).m[0][0];

                        Transform& transform = GetComponent<Transform>(selected_entity);
                        Matrix4 gizmo_matrix = ToMatrix4(transform);

                        ImGuizmo::Manipulate(view, projection, operation, mode, &gizmo_matrix.m[0][0], nullptr, snap_enabled ? &snap : nullptr);

                        if (ImGuizmo::IsUsing() && !isnan(gizmo_matrix.m[0][0]))
                        {
                            float matrix_translation[3], matrix_rotation[3], matrix_scale[3];
             
                            transform.position = { matrix_translation[0], matrix_translation[1], matrix_translation[2] };
                            transform.rotation = { matrix_rotation[0], matrix_rotation[1], matrix_rotation[2] };
                            transform.scale = { matrix_scale[0], matrix_scale[1], matrix_scale[2] };
                            
                            Decompose(gizmo_matrix, transform.position, transform.rotation, transform.scale);
                        }
                    }
                }

                ImGui::End();
                ImGui::PopStyleVar();
            }
        }
    }

    void EndDrawEditor()
    {
        UnbindFrameBuffer();

        ImGuiWindowFlags settings_flags =
            ImGuiWindowFlags_NoDecoration
            | ImGuiWindowFlags_NoDocking
            | ImGuiWindowFlags_AlwaysAutoResize
            | ImGuiWindowFlags_NoFocusOnAppearing
            | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoBackground
            | ImGuiWindowFlags_NoNav;

        ImGui::Begin("Editor Settings", nullptr, settings_flags);
        const char* toggle_button_name = editor->enabled ? "Editor" : "Game";
        if (ImGui::Button(toggle_button_name))
        {
            editor->enabled = !editor->enabled;
            app->im_gui_renderer.use_dockspace = editor->enabled;
        }
        ImGui::End();
    }
}

#endif
