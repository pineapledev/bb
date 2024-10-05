#include "entity.h"

namespace Nit
{
#define NIT_CHECK_ENTITY_REGISTRY_CREATED NIT_CHECK(entity_registry)
    
    EntityRegistry* entity_registry = nullptr;
    
    void SetEntityRegistryInstance(EntityRegistry* entity_registry_instance)
    {
        NIT_CHECK(entity_registry_instance);
        entity_registry = entity_registry_instance;
    }

    EntityRegistry* GetEntityRegistryInstance()
    {
        NIT_CHECK_ENTITY_REGISTRY_CREATED
        return entity_registry;
    }

    ComponentPool* FindComponentPool(const Type* type)
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

    void InitEntityRegistry()
    {
        NIT_CHECK_ENTITY_REGISTRY_CREATED
        for (u32 i = 0; i < MAX_ENTITIES; ++i)
        {
            entity_registry->available_entities.push(i);
        }
    }

    void FinishEntityRegistry()
    {
        NIT_CHECK_ENTITY_REGISTRY_CREATED
        for (u32 i = 0; i < entity_registry->next_component_type_index; ++i)
        {
            ComponentPool& data = entity_registry->component_pool[i];
            FinishPool(&data.data_pool);
        }
    }

    Entity CreateEntity()
    {
        NIT_CHECK_ENTITY_REGISTRY_CREATED
        NIT_CHECK_MSG(entity_registry->entity_count < MAX_ENTITIES, "Entity limit reached!");
        Entity entity = entity_registry->available_entities.front();
        entity_registry->available_entities.pop();
        ++entity_registry->entity_count;
        entity_registry->signatures[entity].set(0, true);
        return entity;
    }

    void DestroyEntity(Entity entity)
    {
        NIT_CHECK_ENTITY_REGISTRY_CREATED
        NIT_CHECK_MSG(IsEntityValid(entity), "Entity is not valid!");
        entity_registry->signatures[entity].reset();
        entity_registry->available_entities.push(entity);

        for (u32 i = 0; i < entity_registry->next_component_type_index; ++i)
        {
            ComponentPool& component_pool = entity_registry->component_pool[i];
            RemovePoolElement(&component_pool.data_pool, entity);
        }
        
        --entity_registry->entity_count;
        entity_registry->signatures[entity].set(0, false);

        for (auto& [signature, group] : entity_registry->entity_groups)
        {
            group.entities.erase(entity);
        }
    }

    bool IsEntityValid(const Entity entity)
    {
        NIT_CHECK_ENTITY_REGISTRY_CREATED
        return entity < MAX_ENTITIES && entity_registry->signatures[entity].test(0);
    }

    void EntitySignatureChanged(Entity entity, EntitySignature new_entity_signature)
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

    EntitySignature CreateEntityGroup(const Array<u64>& type_hashes)
    {
        NIT_CHECK_ENTITY_REGISTRY_CREATED
        NIT_CHECK_MSG(!entity_registry->entity_count, "Create the group before any entity gets created!");
        EntitySignature group_signature = BuildEntitySignature(type_hashes);

        if (entity_registry->entity_groups.count(group_signature) != 0)
        {
            return group_signature;
        }
        
        EntityGroup* group = &entity_registry->entity_groups[group_signature];
        group->signature = group_signature;
        return group_signature;
    }

    EntitySignature BuildEntitySignature(const Array<u64>& type_hashes)
    {
        NIT_CHECK_ENTITY_REGISTRY_CREATED
        EntitySignature group_signature;
        group_signature.set(0, true);
        for (u64 type_hash : type_hashes)
        {
            if (ComponentPool* pool = FindComponentPool(GetType(type_hash)))
            {
                group_signature.set(pool->type_index, true);
            }
        }
        return group_signature;
    }

    EntityGroup& GetEntityGroup(EntitySignature signature)
    {
        NIT_CHECK_ENTITY_REGISTRY_CREATED
        return entity_registry->entity_groups[signature];
    }
}
