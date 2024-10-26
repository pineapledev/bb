#include "editor.h"

#include "core/app.h"
#include "logic/components.h"
#include "logic/scene.h"
#include "render/texture.h"

#ifdef NIT_EDITOR_ENABLED
#include <ImGuizmo.h>
#include "editor_utils.h"

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

        static const char* font_location = "assets/editor/AlbertSans-VariableFont_wght.ttf";
        static u8 font_size = 17;
        const ImGuiIO& io = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF(font_location, font_size);
        
        ImGui::StyleColorsDark();
        ImGuiStyle& style = ImGui::GetStyle();
        style.TabBarOverlineSize = 0.f;
        style.FrameRounding = 9.f;
        style.Colors[ImGuiCol_WindowBg] = { 0.192f, 0.2f, 0.219f, 1.f };
        style.Colors[ImGuiCol_TabUnfocusedActive] = { 0.168f, 0.176f, 0.192f, 1.f };
        style.Colors[ImGuiCol_Tab] = { 0.168f, 0.176f, 0.192f, 1.f };
        style.Colors[ImGuiCol_TabHovered] = { 0.656f, 0.656f, 0.656f, 1.f };
        style.Colors[ImGuiCol_TabActive] = { 0.211f, 0.215f, 0.239f, 1.f };
        style.Colors[ImGuiCol_TitleBg] = { 0.168f, 0.176f, 0.192f, 1.f };
        style.Colors[ImGuiCol_TitleBgActive] = { 0.1f, 0.1f, 0.1f, 1.f };
        style.Colors[ImGuiCol_FrameBg] = { 0.1f, 0.1f, 0.1f, 1.f };
        style.Colors[ImGuiCol_Button] = { 0.356f, 0.356f, 0.416f, 1.f };
        
        RetrieveWindowSize((i32*)&editor->frame_buffer.width, (i32*)&editor->frame_buffer.height);

        editor->frame_buffer.color_attachments = {
            FrameBufferTextureFormat::RGBA8,
            FrameBufferTextureFormat::RED_INTEGER
        };

        editor->frame_buffer.depth_attachment = {
            FrameBufferTextureFormat::Depth
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
                ImGui::MenuItem("Sprite Packer", nullptr, &editor->show_sprite_packer);
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
                        Entity selected = (Entity) entity_id;
                        if ((editor->is_entity_selected = IsEntityValid(selected)))
                        {
                            editor->selected_entity = selected;
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

                        const float* view = &CalculateViewMatrix(camera_transform).n[0];
                        const float* projection = &CalculateProjectionMatrix(camera_data).n[0];

                        Transform& transform = GetComponent<Transform>(selected_entity);
                        
                        Matrix4 gizmo_matrix = CreateTransform(transform.position, transform.rotation, transform.scale);
                        
                        ImGuizmo::Manipulate(view, projection, operation, mode, &gizmo_matrix.n[0], nullptr, snap_enabled ? &snap : nullptr);

                        if (ImGuizmo::IsUsing())
                        {
                            Vector3 position, rotation, scale;
                            Decompose(gizmo_matrix, position, rotation, scale);
                            Vector3 delta_rotation = rotation - transform.rotation; 
                            
                            transform.position = position;
                            transform.rotation += delta_rotation;
                            transform.scale = scale;
                        }
                    }
                }

                ImGui::End();
                ImGui::PopStyleVar();
            }
        }

        if (editor->show_sprite_packer)
        {
            ImGui::Begin("Sprite Packer", &editor->show_sprite_packer);

            // source
            {
                static String source = GetWorkingDirectory().string();
                ImGui::InputFolder(&app->window, "Source", source);
                
                static String dest = GetWorkingDirectory().string();
                ImGui::InputFolder(&app->window, "Destination", dest);

                static String name;
                ImGui::InputText("Asset Name", name);

                if (ImGui::Button("Generate"))
                {
                    // Path relative_source = relative(source, GetWorkingDirectory());
                    // Path relative_dest = relative(dest, GetWorkingDirectory());
                    //
                    // ID texture = CreateAsset<Texture2D>(name, relative_dest.string());
                    // Texture2D* texture_2d = GetAssetDataPtr<Texture2D>(texture);
                    //
                    // LoadTexture2DAsSpriteSheet(texture_2d, name, relative_source.string(), relative_dest.string());
                    // if (texture_2d->sub_texture_count > 0 && !texture_2d->image_path.empty())
                    // {
                    //     SerializeAssetToFile(texture);
                    // }
                    // else
                    // {
                    //     DestroyAsset(texture);
                    // }
                }
            }

            ImGui::End();
        }

        if (editor->show_scene_entities)
        {
            ImGui::Begin("Scene Entities", &editor->show_scene_entities);
            
            AssetPool* pool = GetAssetPool<Scene>();
            u32 num_of_scenes = pool->data_pool.sparse_set.count;
            Scene* scenes = static_cast<Scene*>(pool->data_pool.elements);

            for (u32 i = 0; i < num_of_scenes; ++i)
            {
                Scene* scene = &scenes[i];
                
                if (!scene->loaded)
                {
                    continue;
                }
                
                u32 num_of_entities = (u32) scene->entities.size();
                for (u32 j = 0; j < num_of_entities; ++j)
                {
                    Entity entity = scene->entities[i];
                    
                }
            }

            ImGui::End();
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
