#include "collision_category.h"
#include "collision_flags.h"

#ifdef NIT_EDITOR_ENABLED
#include "editor/editor_utils.h"
#endif

using namespace nit;

void serialize(const CollisionCategory* collider, YAML::Emitter& emitter)
{
    emitter << YAML::Key << "collision_flags" << YAML::Value << collider->collision_flags;
    emitter << YAML::Key << "name" << YAML::Value << collider->name;
}

void deserialize(CollisionCategory* collider, const YAML::Node& node)
{
    if (node["name"])
    {
        collider->name = node["name"].as<String>();
    }

    if (node["collision_flags"])
    {
        collider->collision_flags = node["collision_flags"].as<AssetHandle>();
    }
}

#ifdef NIT_EDITOR_ENABLED
void draw_editor(CollisionCategory* collider)
{
    editor_draw_asset_combo("Collision Flags", type_get<CollisionFlags>(), &collider->collision_flags);
    if (asset_valid(collider->collision_flags))
    {
        CollisionFlags* flags = asset_get_data<CollisionFlags>(collider->collision_flags);
        Array<String> str_flags{flags->count};
        for (u32 i = 0; i < flags->count; ++i)
        {
            str_flags[i] = flags->names[i];
        }
        
        editor_draw_combo("Category", collider->name, str_flags);
        auto data = collision_flags_get_category_data(flags, collider->name);
        collider->category    = data.category;
        collider->mask        = data.mask;
    }
}
#endif

void register_collision_category_component()
{
    component_register<CollisionCategory>({
        .fn_serialize   = serialize,
        .fn_deserialize = deserialize,
        NIT_IF_EDITOR_ENABLED(.fn_draw_editor = draw_editor)
    });
}