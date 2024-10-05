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
    
    ComponentData* FindComponentDataByName(const char* name)
    {
        NIT_CHECK_ENTITY_REGISTRY_CREATED
        for (u32 i = 0; i < entity_registry->next_component_type; ++i)
        {
            ComponentData& data = entity_registry->component_data[i];
            if (data.name == name)
            {
                return &data;
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
        for (u32 i = 0; i < entity_registry->next_component_type; ++i)
        {
            ComponentData& data = entity_registry->component_data[i];
            FinishPool(&data.pool);
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

        for (u32 i = 0; i < entity_registry->next_component_type; ++i)
        {
            ComponentData& data = entity_registry->component_data[i];
            RemovePoolElement(&data.pool, entity);
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

    EntitySignature CreateEntityGroup(const Array<const char*>& types)
    {
        NIT_CHECK_ENTITY_REGISTRY_CREATED
        NIT_CHECK_MSG(!entity_registry->entity_count, "Create the group before any entity gets created!");
        EntitySignature group_signature = BuildEntitySignature(types);

        if (entity_registry->entity_groups.count(group_signature) != 0)
        {
            return group_signature;
        }
        
        EntityGroup* group = &entity_registry->entity_groups[group_signature];
        group->signature = group_signature;
        return group_signature;
    }

    EntitySignature BuildEntitySignature(const Array<const char*>& types)
    {
        NIT_CHECK_ENTITY_REGISTRY_CREATED
        EntitySignature group_signature;
        group_signature.set(0, true);
        for (const char* type_name : types)
        {
            if (ComponentData* data = FindComponentDataByName(type_name))
            {
                group_signature.set(data->type, true);
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
