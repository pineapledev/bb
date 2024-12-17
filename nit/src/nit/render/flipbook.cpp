#include "flipbook.h"
#include "texture.h"
#include "entity/entity.h"

#ifdef NIT_EDITOR_ENABLED
    #include "editor/editor_utils.h"
#endif

namespace nit
{
    void key_deserialize(FlipBook::Key* key, const YAML::Node& node)
    {
        key->name = node["name"].as<String>();
        key->time = node["time"].as<f32>();
    }

    void key_serialize(const FlipBook::Key* key, YAML::Emitter& emitter)
    {
        emitter << YAML::Key << "name" << YAML::Value << key->name;
        emitter << YAML::Key << "time" << YAML::Value << key->time;
    }
    
    void serialize(const FlipBook* flipbook, YAML::Emitter& emitter)
    {
        emitter << YAML::Key << "texture" << YAML::Value << flipbook->texture;
        emitter << YAML::Key << "duration" << YAML::Value << flipbook->duration;
        
        if (flipbook->key_count)
        {
            emitter << YAML::Key << "keys" << YAML::Value << YAML::BeginMap;
            
            for (u32 i = 0; i < flipbook->key_count; ++i)
            {
                const FlipBook::Key* key = flipbook->keys + i;
                emitter << YAML::Key << "key" << YAML::Value << YAML::BeginMap;
                key_serialize(key, emitter);
                emitter << YAML::EndMap;
            }
            emitter << YAML::EndMap;
        }
    }
    
    void deserialize(FlipBook* flipbook, const YAML::Node& node)
    {
        flipbook->texture  = node["texture"].as<AssetHandle>();
        flipbook->duration = node["duration"].as<f32>();
        
        if (const YAML::Node& keys_node = node["keys"])
        {
            flipbook->key_count = 0u;
            for (const auto& keys_node_child : keys_node)
            {
                const YAML::Node& key_node = keys_node_child.second;
                FlipBook::Key* key = flipbook->keys + flipbook->key_count;
                key_deserialize(key, key_node);
                ++flipbook->key_count;
            }
        }
    }

#ifdef NIT_EDITOR_ENABLED
    void draw_editor(FlipBook* flipbook)
    {
        const bool texture_changed = editor_draw_asset_combo("texture", type_get<Texture2D>(), &flipbook->texture);
        
        if (texture_changed && asset_valid(flipbook->texture))
        {
            Texture2D* texture = asset_get_data<Texture2D>(flipbook->texture);
            
            flipbook->key_count = std::min(texture->sub_texture_count + 1, FlipBook::MAX_KEYS);
            
            for (u32 i = 0; i < flipbook->key_count - 1; ++i)
            {
                SubTexture2D* sub_texture = texture->sub_textures + i;
                flipbook->keys[i] = { .name = sub_texture->name, .index = (i32) i, .time = 0.f };
            }
            
            // Por consistencia, añadimos una última clave, que será un duplicado de la primera
            SubTexture2D* sub_texture = texture->sub_textures;
            flipbook->keys[flipbook->key_count - 1] = { .name = sub_texture->name, .index = (i32) flipbook->key_count - 1, .time = 0.f };
        }
        
        if (editor_draw_drag_f32("duration", flipbook->duration) || texture_changed)
        {
            flipbook_adjust_to_duration(flipbook);
        }

        bool should_sort = false;
        
        bool pending_remove = false;
        u32  remove_index = 0.f;
        
        for (u32 i = 0; i < flipbook->key_count; ++i)
        {
            FlipBook::Key* key = flipbook->keys + i;
            
            ImGui::PushID(i);
            
            if (ImGui::TreeNodeEx(key->name.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Spacing();
                
                editor_draw_text("name", key->name.c_str());
                
                if (editor_draw_drag_f32("time", key->time, 0.01f, 0.f, F32_MAX))
                {
                    should_sort = true;
                }

                if (ImGui::Button("Remove"))
                {
                    remove_index = i;
                    pending_remove = true;
                }
                
                ImGui::Separator();
                ImGui::Spacing();
                ImGui::TreePop();
            }
            
            ImGui::PopID();
        }

        if (asset_valid(flipbook->texture) && flipbook->key_count < FlipBook::MAX_KEYS)
        {
            if (editor_draw_centered_button("Add Key"))
            {
                ImGui::OpenPopup("Add Key");
            }

            if (ImGui::BeginPopup("Add Key"))
            {
                Texture2D* texture = asset_get_data<Texture2D>(flipbook->texture);
                
                for (u32 i = 0; i < texture->sub_texture_count; ++i)
                {
                    SubTexture2D* sub_texture = texture->sub_textures + i;
                    
                    if (ImGui::MenuItem(sub_texture->name.c_str()))
                    {
                        FlipBook::Key* new_key = flipbook->keys + flipbook->key_count;
                        new_key->index = flipbook->key_count;
                        new_key->name  = sub_texture->name;
                        new_key->time  = 0.f;
                        ++flipbook->key_count;
                    }
                }
                ImGui::EndPopup();
            }
        }

        if (pending_remove)
        {
            for (u32 i = remove_index; i < flipbook->key_count - 1; ++i)
            {
                flipbook->keys[i] = flipbook->keys[i + 1];
            }

            --flipbook->key_count;
            pending_remove = false;
        }

        if (should_sort)
        {
            flipbook_sort(flipbook);
            flipbook->duration = 0.f;
            for (u32 i = 0; i < flipbook->key_count; ++i)
            {
                FlipBook::Key* key = flipbook->keys + i;
                flipbook->duration += key->time;
            }
        }
    }
#endif
    void flipbook_sort(FlipBook* flipbook)
    {
        if (!flipbook)
        {
            NIT_CHECK(false);
            return;
        }

        const u32 count = flipbook->key_count;
        
        if (!count)
        {
            return;
        }
        
        FlipBook::Key* keys = flipbook->keys;
        
        std::sort(keys, keys + count, [](const FlipBook::Key& a, const FlipBook::Key& b)
        {
           return a.time < b.time; 
        });
    }

    void flipbook_adjust_to_duration(FlipBook* flipbook)
    {
        if (!flipbook)
        {
            NIT_CHECK(false);
            return;
        }

        const u32 count = flipbook->key_count;
        
        if (!count || flipbook->duration <= 0.f)
        {
            return;
        }

        if (count == 1)
        {
            flipbook->keys[0].time = flipbook->duration;
            return;
        }
        
        const f32 key_duration = flipbook->duration / (f32) (flipbook->key_count - 1);
        f32 acc_time = 0.f;
        
        for (u32 i = 0; i < flipbook->key_count; ++i)
        {
            FlipBook::Key* key = flipbook->keys + i;
            key->time = acc_time;
            acc_time += key_duration;
        }
    }

    void register_flipbook_asset()
    {
        asset_register_type<FlipBook>({
            .fn_serialize = serialize,
            .fn_deserialize = deserialize,
            NIT_IF_EDITOR_ENABLED(.fn_draw_editor = draw_editor)
        });
    }

    void serialize_animation(const FlipBookAnimation* animation, YAML::Emitter& emitter)
    {                                         
        emitter << YAML::Key << "flipbook" << YAML::Value << animation->flipbook;
        emitter << YAML::Key << "playing" << YAML::Value << animation->playing;
        emitter << YAML::Key << "loop" << YAML::Value << animation->loop;
    }

    void deserialize_animation(FlipBookAnimation* animation, const YAML::Node& node)
    {
        animation->flipbook = node["flipbook"].as<AssetHandle>();
        animation->playing = node["playing"].as<bool>();
        animation->loop = node["loop"].as<bool>();
    }

#ifdef NIT_EDITOR_ENABLED
    void draw_editor_animation(FlipBookAnimation* animation)
    {
        editor_draw_asset_combo("Flipbook", type_get<FlipBook>(), &animation->flipbook);
        editor_draw_bool("Is Playing", animation->playing);
        editor_draw_bool("Loop", animation->loop);
        editor_draw_text("Time", "%f", animation->time);
        editor_draw_text("Key", "%u", animation->current_key);
    }
#endif
    
    void register_flipbook_animation_component()
    {
        component_register<FlipBookAnimation>({
           .fn_serialize   = serialize_animation,
           .fn_deserialize = deserialize_animation,
           NIT_IF_EDITOR_ENABLED(.fn_draw_editor = draw_editor_animation)
       });
    }
}