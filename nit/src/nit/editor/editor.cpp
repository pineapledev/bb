﻿#include "editor.h"

#include <stack>

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

    void TraverseDirectory(const Path& directory, u32 parent_node, int depth = 0)
    {
        for (const auto& dir_entry : std::filesystem::directory_iterator(directory))
        {
            const Path& dir_path = dir_entry.path();
            const String relative_path = std::filesystem::relative(dir_path, GetAssetsDirectory()).string();
            
            if (dir_entry.is_directory())
            {
                u32 id;
                InsertData(&editor->asset_nodes, id, AssetNode{ .is_dir = true, .path = relative_path, .parent = parent_node, .asset = { .name = dir_path.stem().string() } });
                
                if (AssetNode* parent_node_data = GetData<AssetNode>(&editor->asset_nodes, parent_node))
                {
                    parent_node_data->children.push_back(id);
                }

                TraverseDirectory(dir_path, id, depth + 1);
            }
            else if (dir_path.extension().string() == app->asset_registry.extension)
            {
                AssetHandle handle = FindAssetByName(dir_path.filename().stem().string());

                if (!IsAssetValid(handle))
                {
                    continue;
                }

                u32 id;
                InsertData(&editor->asset_nodes, id, AssetNode{ .is_dir = false, .path = relative_path, .parent = parent_node, .asset = handle });
                AssetNode* parent_node_data = GetData<AssetNode>(&editor->asset_nodes, parent_node);

                if (parent_node_data && parent_node_data->is_dir)
                {
                    parent_node_data->children.push_back(id);
                }
            }
        }
    }

    void InvalidateAssetNodes()
    {
        if (editor->asset_nodes.elements != nullptr)
        {
            Free(&editor->asset_nodes);
        }
        
        Load<AssetNode>(&editor->asset_nodes, 300, true);

        InsertData(&editor->asset_nodes, editor->root_node, AssetNode{ .is_dir = true, .path = "" });
        editor->draw_node = editor->root_node;
        
        TraverseDirectory(GetAssetsDirectory(), editor->root_node);
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

        editor->icons = FindAssetByName("editor_icons");
        
        InvalidateAssetNodes();
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
                        i32 entity_id = ReadFrameBufferPixel(&editor->frame_buffer, 1, mouse_x, mouse_y);
                        Entity selected = (Entity) entity_id;
                        bool valid_entity = IsEntityValid(selected);
                        editor->selection = valid_entity ? Editor::Selection::Entity : Editor::Selection::None;
                        editor->selected_entity = valid_entity ? selected : U32_MAX;
                    }
                }

                auto& camera_group = GetEntityGroup<Camera, Transform>();

                if (!camera_group.entities.empty())
                {
                    Entity camera_entity = *camera_group.entities.begin();

                    // Gizmo stuff
                    Entity selected_entity = editor->selected_entity;
                    
                    if (editor->selection == Editor::Selection::Entity && IsEntityValid(camera_entity) && HasComponent<Transform>(selected_entity))
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
                    Path relative_source = relative(source, GetWorkingDirectory());
                    Path relative_dest = relative(dest, GetWorkingDirectory());
                    
                    AssetHandle texture = CreateAsset<Texture2D>(name, relative_dest.string());
                    Texture2D* texture_2d = GetAssetData<Texture2D>(texture);
                    
                    LoadTexture2DAsSpriteSheet(texture_2d, name, relative_source.string(), relative_dest.string());
                    if (texture_2d->sub_texture_count > 0 && !texture_2d->image_path.empty())
                    {
                        SerializeAssetToFile(texture);
                    }
                    else
                    {
                        DestroyAsset(texture);
                    }
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
            
            if (ImGui::BeginPopupContextWindow())
            {
                if (ImGui::BeginMenu("Open"))
                {
                    for (u32 i = 0; i < num_of_scenes; ++i)
                    {
                        AssetInfo* info = &pool->asset_infos[i];
                        AssetHandle scene_asset = CreateAssetHandle(info);
                
                        if (info->loaded)
                        {
                            continue;
                        }

                        if (ImGui::MenuItem(info->name.c_str()))
                        {
                            DeserializeAssetFromFile(info->path);
                            LoadAsset(scene_asset);
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
                AssetHandle scene_asset = CreateAssetHandle(info);
                
                if (!info->loaded)
                {
                    continue;
                }
                
                const bool is_scene_expanded = ImGui::TreeNodeEx(info->name.c_str(), ImGuiTreeNodeFlags_DefaultOpen);

                ImGui::PushID(i);
                
                if (ImGui::BeginPopupContextItem())
                {
                    if (ImGui::MenuItem("Create Entity"))
                    {
                        Entity entity = CreateEntity();
                        scene->entities.push_back(entity);
                    }
                    if (ImGui::MenuItem("Save"))
                    {
                        SerializeAssetToFile(scene_asset);
                    }
                    if (ImGui::MenuItem("Reload"))
                    {
                        FreeAsset(scene_asset);
                        DeserializeAssetFromFile(info->path);
                        LoadAsset(scene_asset);
                        editor->selection = Editor::Selection::None;
                    }
                    if (ImGui::MenuItem("Close"))
                    {
                        FreeAsset(scene_asset);
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
                    Entity entity = scene->entities[j];

                    Entity selected_entity = editor->selected_entity;

                    const String name = std::to_string(entity);
                    const ImGuiTreeNodeFlags flags = ((IsEntityValid(entity) && selected_entity == entity) ?
                        ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_Leaf;

                    const bool is_entity_expanded = ImGui::TreeNodeEx(name.c_str(), flags);
            
                    if (ImGui::BeginPopupContextItem())
                    {
                        if (ImGui::MenuItem("Destroy Entity"))
                        {
                            editor->selection = Editor::Selection::None;
                            editor->selected_entity = U32_MAX;
                            DestroyEntity(selected_entity);
                            num_of_entities--;
                            scene->entities.erase(std::ranges::find(scene->entities, selected_entity));
                        }
                        if (ImGui::MenuItem("Clone Entity"))
                        {
                            NIT_LOG_TRACE("Not implemented yet!");
                        }
                        ImGui::EndPopup();
                    }
                    
                    if (ImGui::IsItemClicked())
                    {
                        editor->selected_entity = entity;
                        editor->selection = Editor::Selection::Entity;
                    }

                    if (ImGui::IsItemClicked() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                    {
                        NIT_LOG_TRACE("Not implemented yet!");
                    }

                    if (is_entity_expanded)
                    {
                        ImGui::TreePop();
                    }
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
                Entity selected_entity = editor->selected_entity;

                for (u32 i = 0; i < app->entity_registry.next_component_type_index - 1; ++i)
                {
                    ComponentPool* pool = &app->entity_registry.component_pool[i];
                    if (!Invoke(pool->fn_is_in_entity, selected_entity))
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

                    bool remove_component = false;

                    if (ImGui::BeginPopup("Component Settings"))
                    {
                        if (ImGui::MenuItem("Remove Component"))
                        {
                            remove_component = true;
                        }

                        ImGui::EndPopup();
                    }
                    ImGui::PopID();

                    if (is_expanded)
                    {
                        ImGui::Spacing();

                        if (pool->data_pool.type->fn_invoke_draw_editor)
                        {
                            void* data = GetDataRaw(&pool->data_pool, selected_entity);
                            NIT_CHECK(data);
                            DrawEditor(component_type, data);
                        }
                        
                        ImGui::Separator();
                        ImGui::Spacing();
                        ImGui::TreePop();
                    }

                    if (remove_component)
                    {
                        Invoke(pool->fn_remove_from_entity, selected_entity);
                    }
                }

                if (ImGui::CenteredButton("Add Component"))
                {
                    ImGui::OpenPopup("Add Component");
                }

                if (ImGui::BeginPopup("Add Component"))
                {
                    for (u32 i = 0; i < app->entity_registry.next_component_type_index - 1; ++i)
                    {
                        ComponentPool* pool = &app->entity_registry.component_pool[i];
                        if (Invoke(pool->fn_is_in_entity, selected_entity))
                        {
                            continue;
                        }

                        if (ImGui::MenuItem(pool->data_pool.type->name.c_str()))
                        {
                            Invoke(pool->fn_add_to_entity, selected_entity);
                        }
                    }
                    ImGui::EndPopup();
                }
            }
            else if (editor->selection == Editor::Selection::Asset)
            {
                AssetPool* asset_pool = GetAssetPool(editor->selected_asset.type);
                NIT_CHECK(asset_pool);
                
                if (editor->selected_asset.data_id == SparseSet::INVALID)
                {
                    RetargetAssetHandle(editor->selected_asset);
                }

                void* data = GetDataRaw(&asset_pool->data_pool, editor->selected_asset.data_id);
                NIT_CHECK(data);
                
                DrawEditor(editor->selected_asset.type, data);
            }
            ImGui::End();
        }

        if (editor->show_assets)
        {
            ImGui::Begin("Assets", &editor->show_assets);
            {
                if (!IsAssetLoaded(editor->icons))
                {
                    RetainAsset(editor->icons);
                }
                
                if (!IsValid(&editor->asset_nodes, editor->draw_node))
                {
                    NIT_DEBUGBREAK();
                }
                else
                {
                    static auto draw_image_button = [](Editor::Icon icon, f32 size, const char* tag) -> bool
                    {
                        Texture2D* icons = GetAssetData<Texture2D>(editor->icons);
                        ImTextureID icons_id = reinterpret_cast<ImTextureID>(static_cast<u64>(icons->id));
                        V2Verts2D verts_2d;
                        FillQuadVertexUVs(verts_2d, icons->size, icons->sub_textures[(u8) icon].size, icons->sub_textures[(u8) icon].location);
                        Vector2 bottom_left = verts_2d[0];
                        Vector2 top_right   = verts_2d[2];
                        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
                        bool res = ImGui::ImageButton(tag, icons_id, {size, size}, {bottom_left.x, bottom_left.y}, {top_right.x, top_right.y});
                        ImGui::PopStyleColor();
                        return res;
                    };

                    AssetNode* draw_node = GetData<AssetNode>(&editor->asset_nodes, editor->draw_node);

                    static Type* create_asset_type = nullptr;
                
                    if (ImGui::BeginPopupContextWindow())
                    {
                        if (ImGui::BeginMenu("Create"))
                        {
                            for (u32 i = 0; i < app->asset_registry.asset_pools.size(); ++i)
                            {
                                AssetPool* pool = &app->asset_registry.asset_pools[i];
                
                                if (ImGui::MenuItem(pool->data_pool.type->name.c_str()))
                                {
                                    create_asset_type = pool->data_pool.type;
                                }
                            }
                            ImGui::EndMenu();
                        }
                        ImGui::EndPopup();
                    }

                    if (create_asset_type)
                    {
                        ImGui::OpenPopup("Create Asset");
                    }

                    if(ImGui::BeginPopupModal("Create Asset", nullptr))
                    {
                        static String asset_name;
                        ImGui::InputText("name", asset_name);
                        ImGui::Spacing(3);
                    
                        if (ImGui::Button("Create"))
                        {
                            AssetHandle asset = CreateAsset(create_asset_type, asset_name, draw_node->path);
                            u32 id; InsertData(&editor->asset_nodes, id, AssetNode{ .is_dir = false, .path = draw_node->path, .parent = editor->draw_node, .asset = asset});
                            draw_node->children.push_back(id);
                            
                            ImGui::CloseCurrentPopup();
                            create_asset_type = nullptr;
                        }
                        
                        ImGui::SameLine();

                        if (ImGui::Button("Close"))
                        {
                            ImGui::CloseCurrentPopup();
                            create_asset_type = nullptr;
                        }
                        ImGui::EndPopup();
                    }
                    
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
                        AssetNode* node = GetData<AssetNode>(&editor->asset_nodes, node_id);
                        
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
                                    SerializeAssetToFile(editor->selected_asset);
                                    FreeAsset(editor->selected_asset);
                                    DeserializeAssetFromFile(GetAssetInfo(editor->selected_asset)->path);
                                    LoadAsset(editor->selected_asset);
                                }
                                if (ImGui::MenuItem("Reload"))
                                {
                                    FreeAsset(editor->selected_asset);
                                    DeserializeAssetFromFile(GetAssetInfo(editor->selected_asset)->path);
                                    LoadAsset(editor->selected_asset);
                                }
                                if (ImGui::MenuItem("Delete"))
                                {
                                    draw_node->children.erase(std::ranges::find(draw_node->children, node_id));
                                    DestroyAsset(editor->selected_asset);
                                    DeleteData(&editor->asset_nodes, node_id);
                                    editor->selected_asset = {};
                                    editor->selection = Editor::Selection::None;
                                    --i;
                                }
                                ImGui::EndPopup();
                            }
                        }

                        ImGui::CenteredText(node->asset.name.c_str());
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
            stats_text.append("\nTime: "     + std::to_string(app->seconds));
            stats_text.append("\nFrames: "   + std::to_string(app->frame_count));
            stats_text.append("\nFPS: "      + std::to_string(app->frame_count / app->seconds));
            stats_text.append("\nEntities: " + std::to_string(app->entity_registry.entity_count));
            stats_text.append("\nDelta: "    + std::to_string(app->delta_seconds));
            ImGui::Text(stats_text.c_str());
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
