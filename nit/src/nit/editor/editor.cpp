#include <ObjectArray.h>
#ifdef NIT_EDITOR_ENABLED

#include "editor.h"
#include "entity/entity_utils.h"
#include "physics/rigidbody_2d.h"
#include "render/circle.h"
#include "render/line_2d.h"
#include "render/sprite.h"
#include "render/text.h"
#include "core/engine.h"
#include "render/transform.h"
#include "render/camera.h"
#include "entity/scene.h"
#include "render/texture.h"
#include "editor_utils.h"
#include <ImGuizmo.h>

#define NIT_CHECK_EDITOR_CREATED NIT_CHECK_MSG(editor, "Forget to call SetEditorInstance!");

namespace nit
{
    Editor* editor;

    void editor_set_instance(Editor* editor_instance)
    {
        NIT_CHECK(editor_instance);
        editor = editor_instance;
    }

    Editor* editor_get_instance()
    {
        NIT_CHECK_EDITOR_CREATED
        return editor;
    }

    void register_editor()
    {
        entity_create_preset<Transform, Camera>("Camera");
        entity_create_preset<Transform, Sprite>("Sprite");
        entity_create_preset<Transform, Text>("Text");
        entity_create_preset<Transform, Circle>("Circle");
        entity_create_preset<Transform, Line2D>("Line");

        component_register<EditorCameraController>();
    }

    static bool draw_image_button(Editor::Icon icon, f32 size, const char* tag, Vector4 color = V4_COLOR_WHITE)
    {
        Texture2D* icons = asset_get_data<Texture2D>(editor->icons);
        ImTextureID icons_id = reinterpret_cast<ImTextureID>(static_cast<u64>(icons->id));
        V2Verts2D verts_2d;
        fill_quad_vertex_u_vs(verts_2d, icons->size, icons->sub_textures[(u8) icon].size, icons->sub_textures[(u8) icon].location);
        Vector2 top_left     = verts_2d[3];
        Vector2 bottom_right = verts_2d[1];
        ImGui::PushStyleColor(ImGuiCol_Button, { 0.f, 0.f, 0.f, 0.f });
        bool res = ImGui::ImageButton(tag, icons_id, {size, size}, {top_left.x, top_left.y}, {bottom_right.x, bottom_right.y});
        ImGui::PopStyleColor();
        return res;
    };

    void traverse_directory(const Path& directory, u32 parent_node, int depth = 0)
    {
        for (const auto& dir_entry : std::filesystem::directory_iterator(directory))
        {
            const Path& dir_path = dir_entry.path();
            const String relative_path = std::filesystem::relative(dir_path, asset_get_directory()).string();
            
            if (dir_entry.is_directory())
            {
                u32 id;
                pool_insert_data(&editor->asset_nodes, id, AssetNode{ .is_dir = true, .path = relative_path, .parent = parent_node, .asset = { .name = dir_path.stem().string() } });
                
                if (AssetNode* parent_node_data = pool_get_data<AssetNode>(&editor->asset_nodes, parent_node))
                {
                    parent_node_data->children.push_back(id);
                }

                traverse_directory(dir_path, id, depth + 1);
            }
            else if (dir_path.extension().string() == engine_get_instance()->asset_registry.extension)
            {
                AssetHandle handle = asset_find_by_name(dir_path.filename().stem().string());

                if (!asset_valid(handle))
                {
                    continue;
                }

                u32 id;
                pool_insert_data(&editor->asset_nodes, id, AssetNode{ .is_dir = false, .path = relative_path, .parent = parent_node, .asset = handle });
                AssetNode* parent_node_data = pool_get_data<AssetNode>(&editor->asset_nodes, parent_node);

                if (parent_node_data && parent_node_data->is_dir)
                {
                    parent_node_data->children.push_back(id);
                }
            }
        }
    }

    void invalidate_asset_nodes()
    {
        if (editor->asset_nodes.elements != nullptr)
        {
            pool_free(&editor->asset_nodes);
        }
        
        pool_load<AssetNode>(&editor->asset_nodes, 300, true);

        pool_insert_data(&editor->asset_nodes, editor->root_node, AssetNode{ .is_dir = true, .path = "" });
        editor->draw_node = editor->root_node;
        
        traverse_directory(asset_get_directory(), editor->root_node);
    }
    
    void editor_init()
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
        
        window_retrieve_size((i32*)&editor->frame_buffer.width, (i32*)&editor->frame_buffer.height);
        
        editor->frame_buffer.color_attachments = {
            FrameBufferTextureFormat::RGBA8,
            FrameBufferTextureFormat::RED_INTEGER
        };

        editor->frame_buffer.depth_attachment = {
            FrameBufferTextureFormat::Depth
        };

        load_frame_buffer(&editor->frame_buffer);

        editor->icons = asset_find_by_name("editor icons");
        
        invalidate_asset_nodes();

        editor->editor_camera_entity = entity_create();

        auto& transform = entity_add<Transform>(editor->editor_camera_entity);
        transform.position.z = 3.f;
        
        auto& camera = entity_add<Camera>(editor->editor_camera_entity);
        camera.projection = CameraProjection::Orthographic;
        
        auto& controller = entity_add<EditorCameraController> (editor->editor_camera_entity);
        
        controller = {
            .desired_zoom = camera.size,
        };
    }

    void draw_hierarchy(EntityID entity, Scene* scene, u32& num_of_entities)
    {
        EntityID selected_entity = editor->selected_entity;
        String name = entity_get_name(entity);
        
        ImGuiTreeNodeFlags flags = entity_valid(entity) && selected_entity == entity ? ImGuiTreeNodeFlags_Selected: 0;
        Array<EntityID> children;
        
        entity_get_children(entity, children);
        
        if (children.empty())
        {
            flags |= ImGuiTreeNodeFlags_Leaf;
        }

        bool enabled = entity_global_enabled(entity);
        
        if (!enabled)
        {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
        }

        bool opened = ImGui::TreeNodeEx(name.c_str(), flags);

        if (!enabled)
        {
            ImGui::PopStyleColor();
        }

        if (opened)
        {
            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
            {
                ImGui::SetDragDropPayload("ENTITY_DRAG", &entity, sizeof(EntityID));
                ImGui::Text("Drag Entity %d", entity);
                ImGui::EndDragDropSource();
            }

            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ENTITY_DRAG"))
                {
                    EntityID* draggedEntity = (EntityID*)payload->Data;

                    if (*draggedEntity != entity)
                    {
                        entity_set_parent(*draggedEntity, entity);
                    }
                }
                ImGui::EndDragDropTarget();
            }

            if (ImGui::BeginPopupContextItem())
            {
                if (ImGui::MenuItem("Destroy Entity"))
                {
                    editor->selection = Editor::Selection::None;
                    editor->selected_entity = U32_MAX;

                    if (entity_valid(selected_entity))
                    {
                        entity_destroy(selected_entity);
                        num_of_entities--;
                        scene->entities.erase(std::ranges::find(scene->entities, selected_entity));
                    }
                }
                if (ImGui::MenuItem("Clone Entity"))
                {
                    selected_entity = entity_clone(selected_entity);
                    scene->entities.push_back(selected_entity);
                    num_of_entities++;
                }
                ImGui::EndPopup();
            }

            if (ImGui::IsItemClicked())
            {
                editor->selected_entity = entity;
                editor->selection = Editor::Selection::Entity;

                if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                {
                    auto& transform = entity_get<Transform>(entity);
                    auto& camera_transform = entity_get<Transform>(editor->editor_camera_entity);
                    auto& camera_controller = entity_get<EditorCameraController>(
                        editor->editor_camera_entity);
                    auto pos = Vector3{
                        transform.position.x, transform.position.y, camera_transform.position.z
                    };
                    camera_controller.aux_position = pos;
                    camera_transform.position = pos;
                }
            }

            for (EntityID child : children)
            {
                draw_hierarchy(child, scene, num_of_entities);
            }
            
            ImGui::TreePop();
        }
    }
    
    void editor_begin()
    {
        NIT_CHECK_EDITOR_CREATED

        if (!editor->enabled)
        {
            return;
        }
        
        // Update editor camera 
        {
            auto& camera     = entity_get<Camera>(editor->editor_camera_entity); 
            auto& controller = entity_get<EditorCameraController>(editor->editor_camera_entity);
            auto& transform  = entity_get<Transform>(editor->editor_camera_entity);
            
            // Zoom stuff

            if (!controller.is_zooming && abs(ImGui::GetIO().MouseWheel) > F32_EPSILON)
            {
                controller.is_zooming = true;
            }
            
            if (controller.is_zooming && controller.time_zooming < controller.time_to_stop_zoom)
            {
                controller.desired_zoom -= ImGui::GetIO().MouseWheel * controller.zoom_step;
                controller.desired_zoom = std::clamp(controller.desired_zoom, 0.f, F32_MAX);

                if (std::abs(controller.desired_zoom - camera.size) > .01f)
                {
                    const float dir = camera.size < controller.desired_zoom ? 1.f : -1.f;
                    camera.size += controller.zoom_speed * dir * delta_seconds();
                }
                else
                {
                    camera.size = controller.desired_zoom;
                }
                
                controller.time_zooming += delta_seconds();
            }
            else
            {
                controller.is_zooming = false;
                controller.time_zooming = 0.f;
                controller.desired_zoom = camera.size;
            }
            
            // Move
            const bool is_right_mouse_pressed = ImGui::IsMouseDown(ImGuiMouseButton_Right);
            Vector2 cursor_pos = input_get_cursor_position();
            
            Vector2 window_size;
            window_size.x = (f32) engine_get_instance()->editor.frame_buffer.width;
            window_size.y = (f32) engine_get_instance()->editor.frame_buffer.height;
            
            // Mouse pressed
            if (!controller.mouse_down && is_right_mouse_pressed)
            {
                controller.mouse_down = true;
                Matrix4 camera_matrix = camera_proj_view(camera, { controller.aux_position });
                Vector3 mouse_world   = mat_screen_to_world(camera_matrix, cursor_pos, window_size);
                controller.offset_pos = mouse_world + controller.aux_position;
            }

            // Mouse released
            if (controller.mouse_down && !is_right_mouse_pressed)
            {
                controller.mouse_down   = false;
                controller.aux_position = transform.position;
            }

            // Mouse hold
            if (is_right_mouse_pressed)
            {
                Matrix4 camera_matrix = camera_proj_view(camera, { controller.aux_position });
                Vector3 mouse_world   = mat_screen_to_world(camera_matrix, cursor_pos, window_size) * -1;
                transform.position = Vector3{ mouse_world.x, mouse_world.y, transform.position.z } + Vector3{ controller.offset_pos.x, controller.offset_pos.y, 0 };
            }
        }        

        if (engine_get_instance()->im_gui_renderer.is_dockspace_enabled && ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Window"))
            {
                ImGui::MenuItem("Viewport", nullptr, &editor->show_viewport);
                ImGui::MenuItem("Sprite Packer", nullptr, &editor->show_sprite_packer);
                ImGui::MenuItem("Scene Entities", nullptr, &editor->show_scene_entities);
                ImGui::MenuItem("Properties", nullptr, &editor->show_properties);
                ImGui::MenuItem("Assets", nullptr, &editor->show_assets);
                ImGui::MenuItem("Stats", nullptr, &editor->show_stats);
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        // Play / Pause screen
        {
            ImGuiWindowFlags flags = 0;
            flags |= ImGuiWindowFlags_NoTitleBar;
            flags |= ImGuiWindowFlags_AlwaysAutoResize;
            flags |= ImGuiWindowFlags_NoFocusOnAppearing;
            ImGui::SetNextWindowBgAlpha(0.35f);
            ImGui::Begin("#PlayPauseScreen", nullptr, flags);

            if (draw_image_button(Editor::Icon::Play, 16, "play", V4_COLOR_LIGHT_GREEN))
            {
                if (!editor->is_stopped && editor->is_paused)
                {
                    editor->is_paused = false;
                }
                
                if (editor->is_stopped)
                {
                    editor->is_stopped = false;
                }
            }

            ImGui::SameLine();

            if (draw_image_button(Editor::Icon::Stop, 16, "stop", V4_COLOR_LIGHT_RED))
            {
                if (!editor->is_stopped)
                {
                    Array<AssetHandle> all_scenes;
                    Array<AssetHandle> scenes;

                    asset_find_by_type(type_get<Scene>(), all_scenes);

                    editor->selection = Editor::Selection::None;
                    editor->selected_entity = NULL_ENTITY;
                    
                    for (AssetHandle& handle : all_scenes)
                    {
                        if (!asset_loaded(handle))
                        {
                            continue;
                        }

                        scenes.push_back(handle);
                        asset_free(handle);
                    }

                    for (AssetHandle& handle : scenes)
                    {
                        asset_load(handle);
                    }

                    editor->is_stopped = true;
                }
            }

            ImGui::SameLine();

            if (draw_image_button(Editor::Icon::Pause, 16, "pause"))
            {
                editor->is_paused = true;
            }

            ImGui::SameLine();

            if (editor->next_frame)
            {
                editor->next_frame = false;
            }
            
            if (draw_image_button(Editor::Icon::Next, 16, "next"))
            {
                if (editor->is_paused && !editor->next_frame)
                {
                    editor->next_frame = true;
                }
            }

            ImGui::SameLine();

            ImGui::End();
        }
        
        if (editor->show_viewport)
        {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, {0, 0});
            ImGui::Begin("Viewport", &editor->show_viewport);
            {
                bind_frame_buffer(&editor->frame_buffer);

                const ImVec2 viewport_offset = ImGui::GetCursorPos();
                const ImVec2 panel_size = ImGui::GetContentRegionAvail();

                if (const Vector2 viewport_size = {panel_size.x, panel_size.y}; viewport_size != editor->viewport_size)
                {
                    editor->viewport_size = viewport_size;
                    // set draw system screen size
                    resize_frame_buffer(&editor->frame_buffer, (u32)viewport_size.x, (u32)viewport_size.y);
                    bind_frame_buffer(&editor->frame_buffer);
                }

                const ImTextureID fb_id = reinterpret_cast<ImTextureID>(static_cast<u64>(editor->frame_buffer.color_attachment_ids[0]));
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
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGuizmo::IsOver() && (ImGui::IsWindowFocused() || ImGui::IsWindowHovered()))
                {
                    if (mouse_x >= 0 && mouse_y >= 0 && mouse_x < (i32)editor->viewport_size.x && mouse_y < (i32)editor
                        ->viewport_size.y)
                    {
                        i32 entity_id = read_frame_buffer_pixel(&editor->frame_buffer, 1, mouse_x, mouse_y);
                        EntityID selected = (EntityID) entity_id;
                        bool valid_entity = entity_valid(selected);
                        editor->selection = valid_entity ? Editor::Selection::Entity : Editor::Selection::None;
                        editor->selected_entity = valid_entity ? selected : U32_MAX;
                    }
                }

                auto& camera_group = entity_get_group<Camera, Transform>();

                if (!camera_group.entities.empty())
                {
                    EntityID camera_entity = get_main_camera();

                    // Gizmo stuff
                    EntityID selected_entity = editor->selected_entity;
                    
                    if (entity_valid(selected_entity) && editor->selection == Editor::Selection::Entity && entity_valid(camera_entity) && entity_has<Transform>(selected_entity))
                    {
                        auto& camera_data      = entity_get<Camera>(camera_entity);
                        camera_data.aspect = engine_window_size().x / engine_window_size().y;
                        
                        auto& camera_transform = entity_get<Transform>(camera_entity);
                        
                        ImGuizmo::SetOrthographic(camera_data.projection == CameraProjection::Orthographic);
                        ImGuizmo::SetDrawlist();
                        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

                        if (ImGui::GetWindowWidth() < 1024)
                        {
                            printf("");
                        }
                        
                        static ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
                        static ImGuizmo::MODE mode = ImGuizmo::LOCAL;
                        static bool snap_enabled = false;
                        static float snap = 0;
                        
                        if (ImGui::IsKeyPressed(ImGuiKey_W)) operation = ImGuizmo::TRANSLATE;
                        if (ImGui::IsKeyPressed(ImGuiKey_E)) operation = ImGuizmo::ROTATE;
                        if (ImGui::IsKeyPressed(ImGuiKey_R)) operation = ImGuizmo::SCALE;

                        const float* view = &camera_view(camera_transform).n[0];
                        const float* projection = &camera_proj(camera_data).n[0];

                        Transform& transform = entity_get<Transform>(selected_entity);
                        
                        Matrix4 gizmo_matrix = mat_create_transform(transform.position, transform.rotation, transform.scale);
                        
                        ImGuizmo::Manipulate(view, projection, operation, mode, &gizmo_matrix.n[0], nullptr, snap_enabled ? &snap : nullptr);
                        
                        if (ImGuizmo::IsUsing() && !editor->is_using_gizmo)
                        {
                            if (entity_has<Rigidbody2D>(selected_entity))
                            {
                                entity_get<Rigidbody2D>(selected_entity).follow_transform = true;
                            }
                            
                            editor->is_using_gizmo = true;
                        }
                        else if (!ImGuizmo::IsUsing() && editor->is_using_gizmo)
                        {
                            if (entity_has<Rigidbody2D>(selected_entity))
                            {
                                entity_get<Rigidbody2D>(selected_entity).follow_transform = false;
                            }
                            
                            editor->is_using_gizmo = false;
                        }
                        
                        if (ImGuizmo::IsUsing())
                        {
                            Vector3 position, rotation, scale;
                            mat_decompose(gizmo_matrix, position, rotation, scale);
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
                static String source;
                editor_draw_input_text("Source", source);
                
                static String dest;
                editor_draw_input_text("Destination", dest);

                static String name;
                editor_draw_input_text("Asset Name", name);

                if (ImGui::Button("Generate"))
                {
                    AssetHandle texture = asset_create<Texture2D>(name, dest);
                    Texture2D* texture_2d = asset_get_data<Texture2D>(texture);
                    
                    texture_2d_load(texture_2d, name, source, dest);
                    
                    if (texture_2d->sub_texture_count > 0 && !texture_2d->image_path.empty())
                    {
                        asset_serialize_to_file(texture);
                    }
                    else
                    {
                        asset_destroy(texture);
                    }
                }
            }

            ImGui::End();
        }

        if (editor->show_scene_entities)
        {
            ImGui::Begin("Scene Entities", &editor->show_scene_entities);
            
            AssetPool* pool = asset_get_pool<Scene>();
            u32 num_of_scenes = pool->data_pool.sparse_set.count;
            Scene* scenes = static_cast<Scene*>(pool->data_pool.elements);
            
            if (ImGui::BeginPopupContextWindow())
            {
                if (ImGui::BeginMenu("Open"))
                {
                    for (u32 i = 0; i < num_of_scenes; ++i)
                    {
                        AssetInfo* info = &pool->asset_infos[i];
                        AssetHandle scene_asset = asset_create_handle(info);
                
                        if (info->loaded)
                        {
                            continue;
                        }

                        if (ImGui::MenuItem(info->name.c_str()))
                        {
                            asset_deserialize_from_file(info->path);
                            asset_load(scene_asset);
                        }
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndPopup();
            }
            
            for (u32 i = 0; i < num_of_scenes; ++i)
            {
                Scene* scene = &scenes[i];
                AssetInfo* info = &pool->asset_infos[i];
                AssetHandle scene_asset = asset_create_handle(info);
                
                if (!info->loaded)
                {
                    continue;
                }
                
                const bool is_scene_expanded = ImGui::TreeNodeEx(info->name.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

                ImGui::PushID(i);
                
                if (ImGui::BeginPopupContextItem())
                {
                    if (ImGui::BeginMenu("Create"))
                    {
                        if (ImGui::MenuItem("Empty entity"))
                        {
                            EntityID entity = entity_create();
                            scene->entities.push_back(entity);
                        }

                        for (auto& [name, preset] : entity_registry_get_instance()->entity_presets)
                        {
                            if (ImGui::MenuItem(name.c_str()))
                            {
                                EntityID entity = entity_create_from_preset(name);
                                scene->entities.push_back(entity);
                            }
                        }
                        ImGui::EndMenu();
                    }
                    
                    if (ImGui::MenuItem("Save"))
                    {
                        asset_serialize_to_file(scene_asset);
                    }
                    if (ImGui::MenuItem("Reload"))
                    {
                        asset_free(scene_asset);
                        asset_deserialize_from_file(info->path);
                        asset_load(scene_asset);
                        editor->selection = Editor::Selection::None;
                    }
                    if (ImGui::MenuItem("Close"))
                    {
                        asset_free(scene_asset);
                        editor->selection = Editor::Selection::None;
                    }
                    ImGui::EndPopup();
                }

                ImGui::PopID();

                if (!is_scene_expanded)
                {
                    continue;
                }

                u32 num_of_entities = (u32) scene->entities.size();
                
                for (u32 j = 0; j < num_of_entities; ++j)
                {
                    EntityID entity = scene->entities[j];
                    
                    if (entity_valid(entity_get_parent(entity)))
                    {
                        continue;
                    }

                    draw_hierarchy(entity, scene, num_of_entities);
                }

                if (is_scene_expanded)
                {
                    ImGui::TreePop();
                }
            }

            ImGui::End();
        }

        if (editor->show_properties)
        {
            ImGui::Begin("Properties", &editor->show_properties);
            
            if (editor->selection == Editor::Selection::Entity)
            {
                EntityID selected_entity = editor->selected_entity;
                
                String name = entity_get_name(selected_entity);
                
                if (editor_draw_input_text("Name", name))
                {
                    entity_set_name(selected_entity, name);
                }

                bool enabled = entity_enabled(selected_entity);
                
                if (editor_draw_bool("Enabled", enabled))
                {
                    entity_set_enabled(selected_entity, enabled);
                }
                
                editor_draw_text("UUID", "%llu", (u64) entity_get_uuid(selected_entity));
                editor_draw_text("Entity ID", "%u", selected_entity);

                ImGui::Separator();
                ImGui::Spacing();
                
                for (u32 i = 0; i < engine_get_instance()->entity_registry.next_component_type_index - 1; ++i)
                {
                    ComponentPool* pool = &engine_get_instance()->entity_registry.component_pool[i];
                    if (!delegate_invoke(pool->fn_is_in_entity, selected_entity))
                    {
                        continue;
                    }

                    Type* component_type = pool->data_pool.type;
                    
                    bool is_expanded = ImGui::TreeNodeEx(component_type->name.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

                    ImGui::SameLine();

                    ImGui::PushID(i);
                    if (ImGui::Button("+"))
                    {
                        ImGui::OpenPopup("Component Settings");
                    }

                    bool remove_entity = false;

                    if (ImGui::BeginPopup("Component Settings"))
                    {
                        if (ImGui::MenuItem("Remove Component"))
                        {
                            remove_entity = true;
                        }

                        ImGui::EndPopup();
                    }
                    ImGui::PopID();

                    if (is_expanded)
                    {
                        ImGui::Spacing();

                        if (pool->data_pool.type->fn_invoke_draw_editor)
                        {
                            void* data = pool_get_raw_data(&pool->data_pool, selected_entity);
                            NIT_CHECK(data);
                            type_draw_editor(component_type, data);
                        }
                        
                        ImGui::Separator();
                        ImGui::Spacing();
                        ImGui::TreePop();
                    }

                    if (remove_entity)
                    {
                        delegate_invoke(pool->fn_remove_from_entity, selected_entity);
                    }
                }

                if (editor_draw_centered_button("Add Component"))
                {
                    ImGui::OpenPopup("Add Component");
                }

                if (ImGui::BeginPopup("Add Component"))
                {
                    for (u32 i = 0; i < engine_get_instance()->entity_registry.next_component_type_index - 1; ++i)
                    {
                        ComponentPool* pool = &engine_get_instance()->entity_registry.component_pool[i];
                        if (delegate_invoke(pool->fn_is_in_entity, selected_entity))
                        {
                            continue;
                        }

                        if (ImGui::MenuItem(pool->data_pool.type->name.c_str()))
                        {
                            void* null_data = nullptr;
                            delegate_invoke(pool->fn_add_to_entity, selected_entity, null_data, true);
                        }
                    }
                    ImGui::EndPopup();
                }
            }
            else if (editor->selection == Editor::Selection::Asset)
            {
                AssetPool* asset_pool = asset_get_pool(editor->selected_asset.type);
                NIT_CHECK(asset_pool);
                
                if (editor->selected_asset.data_id == SparseSet::INVALID)
                {
                    asset_retarget_handle(editor->selected_asset);
                }

                void* data = pool_get_raw_data(&asset_pool->data_pool, editor->selected_asset.data_id);
                NIT_CHECK(data);
                
                type_draw_editor(editor->selected_asset.type, data);
            }
            ImGui::End();
        }

        if (editor->show_assets)
        {
            ImGui::Begin("Assets", &editor->show_assets);
            {
                if (!asset_loaded(editor->icons))
                {
                    asset_retain(editor->icons);
                }
                
                if (!pool_is_valid(&editor->asset_nodes, editor->draw_node))
                {
                    NIT_DEBUGBREAK();
                }
                else
                {
                    AssetNode* draw_node = pool_get_data<AssetNode>(&editor->asset_nodes, editor->draw_node);

                    static Type* asset_create_type = nullptr;
                
                    if (ImGui::BeginPopupContextWindow())
                    {
                        if (ImGui::BeginMenu("Create"))
                        {
                            for (u32 i = 0; i < engine_get_instance()->asset_registry.asset_pools.size(); ++i)
                            {
                                AssetPool* pool = &engine_get_instance()->asset_registry.asset_pools[i];
                
                                if (ImGui::MenuItem(pool->data_pool.type->name.c_str()))
                                {
                                    asset_create_type = pool->data_pool.type;
                                }
                            }
                            ImGui::EndMenu();
                        }
                        ImGui::EndPopup();
                    }

                    if (asset_create_type)
                    {
                        ImGui::OpenPopup("Create Asset");
                    }

                    ImGui::PushStyleColor(ImGuiCol_PopupBg, {0.192f, 0.2f, 0.219f, 1.f});
                    if(ImGui::BeginPopupModal("Create Asset", nullptr))
                    {
                        static char asset_name[500];
                        ImGui::InputText("##name", asset_name, 500);
                        editor_draw_spacing(3);
                        
                        if (ImGui::Button("Create"))
                        {
                            AssetHandle asset = asset_create(asset_create_type, asset_name, draw_node->path);
                            u32 id; pool_insert_data(&editor->asset_nodes, id, AssetNode{ .is_dir = false, .path = draw_node->path, .parent = editor->draw_node, .asset = asset});
                            draw_node->children.push_back(id);
                            
                            ImGui::CloseCurrentPopup();
                            asset_create_type = nullptr;
                        }
                        
                        ImGui::SameLine();

                        if (ImGui::Button("Close"))
                        {
                            ImGui::CloseCurrentPopup();
                            asset_create_type = nullptr;
                        }
                        ImGui::EndPopup();
                    }
                    ImGui::PopStyleColor();
                    
                    if (editor->draw_node != editor->root_node)
                    {
                        if (ImGui::Button("<-"))
                        {
                            editor->draw_node = draw_node->parent;
                        }
                    }

                    static f32 padding = 16.f;
                    static f32 thumbnail_size = 84.f;
                    f32 cell_size = thumbnail_size + padding;
    
                    f32 panel_width = ImGui::GetContentRegionAvail().x;
                    i32 column_count = static_cast<i32>(panel_width / cell_size);
                    if (column_count < 1) column_count = 1;
    
                    ImGui::Columns(column_count, nullptr, false);
                    
                    for (u32 i = 0; i < draw_node->children.size(); ++i)
                    {
                        u32 node_id = draw_node->children[i];
                        AssetNode* node = pool_get_data<AssetNode>(&editor->asset_nodes, node_id);
                        
                        if (!node)
                        {
                            NIT_DEBUGBREAK();
                            continue;
                        }

                        if (node->is_dir)
                        {
                            if (draw_image_button(Editor::Icon::Folder, thumbnail_size, std::to_string(node_id).c_str()))
                            {
                                editor->draw_node = node_id;
                                break;
                            }
                        }
                        else
                        {
                            if (draw_image_button(Editor::Icon::File, thumbnail_size, std::to_string(node_id).c_str()))
                            {
                                editor->selection = Editor::Selection::Asset;
                                editor->selected_asset = node->asset;
                            }

                            if (ImGui::BeginPopupContextItem())
                            {
                                editor->selection = Editor::Selection::Asset;
                                editor->selected_asset = node->asset;
                                
                                if (ImGui::MenuItem("Save"))
                                {
                                    asset_serialize_to_file(editor->selected_asset);
                                    asset_free(editor->selected_asset);
                                    asset_deserialize_from_file(asset_get_info(editor->selected_asset)->path);
                                    asset_load(editor->selected_asset);
                                }
                                if (ImGui::MenuItem("Reload"))
                                {
                                    asset_free(editor->selected_asset);
                                    asset_deserialize_from_file(asset_get_info(editor->selected_asset)->path);
                                    asset_load(editor->selected_asset);
                                }
                                if (ImGui::MenuItem("Delete"))
                                {
                                    draw_node->children.erase(std::ranges::find(draw_node->children, node_id));
                                    asset_destroy(editor->selected_asset);
                                    pool_delete_data(&editor->asset_nodes, node_id);
                                    editor->selected_asset = {};
                                    editor->selection = Editor::Selection::None;
                                    --i;
                                }
                                ImGui::EndPopup();
                            }
                        }

                        editor_draw_centered_text(node->asset.name.c_str());
                        ImGui::NextColumn();
                    }
                }
            }
            ImGui::End();
        }

        if (editor->show_stats)
        {
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing;
            ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background

            ImGui::Begin("Stats", &editor->show_stats, window_flags);
            String stats_text;
            stats_text.append("\nTime: "     + std::to_string(engine_get_instance()->seconds));
            stats_text.append("\nFrames: "   + std::to_string(engine_get_instance()->frame_count));
            stats_text.append("\nFPS: "      + std::to_string(1.f / delta_seconds()));
            stats_text.append("\nEntities: " + std::to_string(engine_get_instance()->entity_registry.entity_count));
            stats_text.append("\nDelta: "    + std::to_string(delta_seconds()));
            ImGui::Text(stats_text.c_str());
            ImGui::End();
        }
    }

    void editor_end()
    {
        unbind_frame_buffer();

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
            engine_get_instance()->im_gui_renderer.use_dockspace = editor->enabled;
        }
        ImGui::End();
    }
}

#endif
