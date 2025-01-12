#include "entity.h"

#include "physics/box_collider_2d.h"
#include "physics/circle_collider.h"
#include "physics/rigidbody_2d.h"
#include "render/transform.h"

namespace nit
{
#define NIT_CHECK_ENTITY_REGISTRY_CREATED NIT_CHECK(entity_registry)
    
    EntityRegistry* entity_registry = nullptr;

    void entity_registry_set_instance(EntityRegistry* entity_registry_instance)
    {
        NIT_CHECK(entity_registry_instance);
        entity_registry = entity_registry_instance;
    }

    EntityRegistry* entity_registry_get_instance()
    {
        NIT_CHECK_ENTITY_REGISTRY_CREATED
        return entity_registry;
    }

    ComponentPool* entity_find_component_pool(const Type* type)
    {
        NIT_CHECK_ENTITY_REGISTRY_CREATED
        for (u32 i = 0; i < entity_registry->next_component_type_index; ++i)
        {
            ComponentPool& component_pool = entity_registry->component_pool[i];
            if (component_pool.data_pool.type == type)
            {
                return &component_pool;
            }
        }
        return nullptr;
    }

    EntityID entity_clone(EntityID entity, const Vector3& position)
    {
        if (!entity_valid(entity))
        {
            return NULL_ENTITY;
        }

        EntityID cloned_entity = entity_create();

        for (u32 i = 0; i < entity_registry->next_component_type_index - 1; ++i)
        {
            ComponentPool* pool = &entity_registry->component_pool[i];
            
            if (!delegate_invoke(pool->fn_is_in_entity, entity))
            {
                continue;
            }
            
            void* component_data = delegate_invoke(pool->fn_get_from_entity, entity);
            
            delegate_invoke(pool->fn_add_to_entity, cloned_entity, component_data, true);
            
            //Turbo hack
            if (pool->data_pool.type == type_get<Transform>())
            {
                auto& transform = entity_get<Transform>(cloned_entity);
                transform.position = position;
            }
            else if (pool->data_pool.type == type_get<Rigidbody2D>())
            {
                auto& rb = entity_get<Rigidbody2D>(cloned_entity);
                rb.invalidated = false;
                rb.handle = {};
            }
            else if (pool->data_pool.type == type_get<BoxCollider2D>())
            {
                auto& collider = entity_get<BoxCollider2D>(cloned_entity);
                collider.invalidated = false;
                collider.handle = {};
            }
            else if (pool->data_pool.type == type_get<CircleCollider>())
            {
                auto& collider = entity_get<CircleCollider>(cloned_entity);
                collider.invalidated = false;
                collider.handle = {};
            }
            
            ComponentAddedArgs args;
            args.entity = cloned_entity;
            args.type = pool->data_pool.type;
            event_broadcast<const ComponentAddedArgs&>(entity_registry_get_instance()->component_added_event, args);
        }

        return cloned_entity;
    }

    EntitySignature entity_get_signature(EntityID entity)
    {
        NIT_CHECK_ENTITY_REGISTRY_CREATED
        if (entity_valid(entity))
        {
            NIT_CHECK_MSG(false, "Trying to get signature from non existent entity!");
            return {};
        }

        return pool_get_data<EntityData>(&entity_registry->entities, entity)->signature;
    }

    void entity_registry_init()
    {
        NIT_CHECK_ENTITY_REGISTRY_CREATED

        pool_load<EntityData>(&entity_registry->entities, entity_registry->max_entities);
        entity_registry->component_pool = new ComponentPool[NIT_MAX_COMPONENT_TYPES];
    }

    void entity_registry_finish()
    {
        NIT_CHECK_ENTITY_REGISTRY_CREATED
        for (u32 i = 0; i < entity_registry->next_component_type_index; ++i)
        {
            ComponentPool& data = entity_registry->component_pool[i];
            pool_free(&data.data_pool);
        }
    }

    EntityID entity_create()
    {
        NIT_CHECK_ENTITY_REGISTRY_CREATED
        NIT_CHECK_MSG(entity_registry->entity_count < entity_registry->max_entities, "Entity limit reached!");
        EntityID entity; pool_insert_data(&entity_registry->entities, entity);
        ++entity_registry->entity_count;
        EntityData* data = pool_get_data<EntityData>(&entity_registry->entities, entity);
        data->id = entity;
        data->name = String("Entity ").append(std::to_string(entity));
        data->uuid = uuid_generate();
        data->signature.set(0, true);
        return entity;
    }

    void entity_destroy(EntityID entity)
    {
        NIT_CHECK_ENTITY_REGISTRY_CREATED
        NIT_CHECK_MSG(entity_valid(entity), "Entity is not valid!");

        for (u32 i = 0; i < entity_registry->next_component_type_index; ++i)
        {
            ComponentPool& component_pool = entity_registry->component_pool[i];

            if (!pool_get_data<EntityData>(&entity_registry->entities, entity)->signature.test(i + 1))
            {
                continue;
            }

            event_broadcast<const ComponentRemovedArgs&>(entity_registry->component_removed_event, {entity, component_pool.data_pool.type});
        }
        
        for (u32 i = 0; i < entity_registry->next_component_type_index; ++i)
        {
            ComponentPool& component_pool = entity_registry->component_pool[i];

            if (!pool_get_data<EntityData>(&entity_registry->entities, entity)->signature.test(i + 1))
            {
                continue;
            }

            pool_delete_data(&component_pool.data_pool, entity);
        }
        
        *pool_get_data<EntityData>(&entity_registry->entities, entity) = {};
        pool_delete_data(&entity_registry->entities, entity);
        
        --entity_registry->entity_count;

        for (auto& [signature, group] : entity_registry->entity_groups)
        {
            group.entities.erase(entity);
        }
    }

    bool entity_valid(const EntityID entity)
    {
        NIT_CHECK_ENTITY_REGISTRY_CREATED
        return entity < entity_registry->max_entities && pool_is_valid(&entity_registry->entities, entity);
    }

    void entity_signature_changed(EntityID entity, EntitySignature new_entity_signature)
    {
        NIT_CHECK_ENTITY_REGISTRY_CREATED
        for (auto& [signature, group] : entity_registry->entity_groups)
        {
            if ((signature | new_entity_signature) == new_entity_signature)
            {
                group.entities.insert(entity);
                continue;
            }

            group.entities.erase(entity);
        }
    }

    EntitySignature entity_create_group(const Array<u64>& type_hashes)
    {
        NIT_CHECK_ENTITY_REGISTRY_CREATED
        NIT_CHECK_MSG(!entity_registry->entity_count, "Create the group before any entity gets created!");
        EntitySignature group_signature = entity_build_signature(type_hashes);

        if (entity_registry->entity_groups.count(group_signature) != 0)
        {
            return group_signature;
        }
        
        EntityGroup* group = &entity_registry->entity_groups[group_signature];
        group->signature = group_signature;
        return group_signature;
    }

    void entity_create_preset(const String& name, const Array<u64>& type_hashes)
    {
        NIT_CHECK_ENTITY_REGISTRY_CREATED
        if (entity_registry->entity_presets.count(name) != 0)
        {
            NIT_CHECK_MSG(false, "There is already a preset called %s", name.c_str());
            return;
        }

        entity_registry->entity_presets[name] = type_hashes;
    }

    EntitySignature entity_build_signature(const Array<u64>& type_hashes)
    {
        NIT_CHECK_ENTITY_REGISTRY_CREATED
        EntitySignature group_signature;
        group_signature.set(0, true);
        for (u64 type_hash : type_hashes)
        {
            if (ComponentPool* pool = entity_find_component_pool(type_get(type_hash)))
            {
                group_signature.set(pool->type_index, true);
            }
        }
        return group_signature;
    }
    
    const String& entity_get_name(EntityID entity)
    {
        NIT_CHECK_ENTITY_REGISTRY_CREATED
        EntityData* data = pool_get_data<EntityData>(&entity_registry->entities, entity);
        return data->name;
    }

    void entity_set_name(EntityID entity, const String& name)
    {
        NIT_CHECK_ENTITY_REGISTRY_CREATED
        EntityData* data = pool_get_data<EntityData>(&entity_registry->entities, entity);
        data->name = name;
    }

    UUID entity_get_uuid(EntityID entity)
    {
        NIT_CHECK_ENTITY_REGISTRY_CREATED
        EntityData* data = pool_get_data<EntityData>(&entity_registry->entities, entity);
        return data->uuid;
    }

    void entity_add_child(EntityID entity)
    {
        NIT_CHECK_ENTITY_REGISTRY_CREATED
        EntityData* data = pool_get_data<EntityData>(&entity_registry->entities, entity);
        auto it = std::ranges::find(data->children, entity);
        if (it != data->children.end())
        {
            return;
        }
        data->children.push_back(entity);
    }

    void entity_get_children(EntityID entity, Array<EntityID>& children)
    {
        NIT_CHECK_ENTITY_REGISTRY_CREATED
        EntityData* data = pool_get_data<EntityData>(&entity_registry->entities, entity);
        children = data->children;
    }

    EntityID entity_get_parent(EntityID entity)
    {
        NIT_CHECK_ENTITY_REGISTRY_CREATED
        EntityData* data = pool_get_data<EntityData>(&entity_registry->entities, entity);
        return data->parent;
    }

    void entity_remove_child(EntityID entity)
    {
        NIT_CHECK_ENTITY_REGISTRY_CREATED
        EntityData* data = pool_get_data<EntityData>(&entity_registry->entities, entity);
        auto it = std::ranges::find(data->children, entity);
        if (it != data->children.end())
        {
            data->children.erase(it);
        }
    }

    EntityArray entity_get_alive_entities()
    {
        NIT_CHECK_ENTITY_REGISTRY_CREATED
        return { (EntityData*) entity_registry->entities.elements, entity_registry->entity_count };
    }

    EntityGroup& entity_get_group(EntitySignature signature)
    {
        NIT_CHECK_ENTITY_REGISTRY_CREATED
        return entity_registry->entity_groups[signature];
    }

    EntityID entity_create_from_preset(const String& name)
    {
        NIT_CHECK_ENTITY_REGISTRY_CREATED

        if (entity_registry->entity_presets.count(name) == 0)
        {
            NIT_CHECK_MSG(false, "There is not a preset called %s", name.c_str());
            return NULL_ENTITY;
        }
        
        auto& type_hashes = entity_registry->entity_presets[name];

        EntityID entity = entity_create();
        
        for (u64 type_hash : type_hashes)
        {
            if (ComponentPool* pool = entity_find_component_pool(type_get(type_hash)))
            {
                void* null_data = nullptr;
                delegate_invoke(pool->fn_add_to_entity, entity, null_data, true);
            }
        }

        return entity;
    }

    void entity_serialize(EntityID entity, YAML::Emitter& emitter)
    {
        emitter << YAML::Key << "Entity" << YAML::Value << YAML::BeginMap;

        emitter << YAML::Key << "Name" << YAML::Value << entity_get_name(entity);
        emitter << YAML::Key << "UUID" << YAML::Value << (u64) entity_get_uuid(entity);
        
        for (u8 i = 0; i < entity_registry->next_component_type_index - 1; ++i)
        {
            auto& component_pool = entity_registry->component_pool[i];
            auto& data_pool = component_pool.data_pool;
            
            if (!data_pool.type->fn_invoke_deserialize
                || !data_pool.type->fn_invoke_serialize
                || !delegate_invoke(component_pool.fn_is_in_entity, entity))
            {
                continue;
            }

            emitter << YAML::Key << data_pool.type->name << YAML::Value << YAML::BeginMap;
                
            void* raw_data = pool_get_raw_data(&data_pool, entity);
            serialize(data_pool.type, raw_data, emitter);
                
            emitter << YAML::EndMap;
        }
        
        Array<EntityID> children;
        entity_get_children(entity, children);
        
        if (!children.empty())
        {
            emitter << YAML::Key << "Children" << YAML::Value << YAML::BeginMap;

            for (EntityID child : children)
            {
                entity_serialize(child, emitter);
            }

            emitter << YAML::EndMap;
        }

        emitter << YAML::EndMap;
    }

    EntityID entity_deserialize(const YAML::Node& node)
    {
        if (!node)
        {
            return 0;
        }

        EntityID entity = entity_create();

        EntityData* data = pool_get_data<EntityData>(&entity_registry->entities, entity);
        
        if (node["Name"])
        {
            data->name = node["Name"].as<String>();
        }

        if (node["UUID"])
        {
            data->uuid = { node["UUID"].as<u64>() };
        }
        
        for (const auto& entity_node_child : node)
        {
            const YAML::Node& component_node = entity_node_child.second;
            String type_name = entity_node_child.first.as<String>();
            
            if (type_name == "Name" || type_name == "UUID")
            {
                continue;
            }

            if (type_name == "Children")
            {
                for (const auto& child : entity_node_child.second)
                {
                    entity_deserialize(child.second);
                }
                
                continue;
            }
            
            auto* component_pool = entity_find_component_pool(type_get(type_name));
            auto& data_pool = component_pool->data_pool;
            void* null_data = nullptr;
            delegate_invoke(component_pool->fn_add_to_entity, entity, null_data, false);
            void* component_data = delegate_invoke(component_pool->fn_get_from_entity, entity);
            deserialize(data_pool.type, component_data, component_node);
            ComponentAddedArgs args;
            args.entity = entity;
            args.type = component_pool->data_pool.type;
            event_broadcast<const ComponentAddedArgs&>(entity_registry_get_instance()->component_added_event, args);
        }
        
        return entity;
    }
}
