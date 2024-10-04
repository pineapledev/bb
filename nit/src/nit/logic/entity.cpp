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
        for (u32 i = 0; i < GetEntityRegistryInstance()->next_component_type; ++i)
        {
            ComponentData& data = GetEntityRegistryInstance()->component_data[i];
            if (data.name == name)
            {
                return &data;
            }
        }
        return nullptr;
    }

    void InitEntityRegistry()
    {
        for (u32 i = 0; i < MAX_ENTITIES; ++i)
        {
            GetEntityRegistryInstance()->available_entities.push(i);
        }
    }

    void FinishEntityRegistry()
    {
        for (u32 i = 0; i < GetEntityRegistryInstance()->next_component_type; ++i)
        {
            ComponentData& data = GetEntityRegistryInstance()->component_data[i];
            FinishPool(&data.pool);
        }
    }

    Entity CreateEntity()
    {
        NIT_CHECK_MSG(GetEntityRegistryInstance()->entity_count < MAX_ENTITIES, "Entity limit reached!");
        Entity entity = GetEntityRegistryInstance()->available_entities.front();
        GetEntityRegistryInstance()->available_entities.pop();
        ++GetEntityRegistryInstance()->entity_count;
        GetEntityRegistryInstance()->signatures[entity].set(0, true);
        return entity;
    }

    void DestroyEntity(Entity entity)
    {
        NIT_CHECK_MSG(IsEntityValid(entity), "Entity is not valid!");
        GetEntityRegistryInstance()->signatures[entity].reset();
        GetEntityRegistryInstance()->available_entities.push(entity);

        for (u32 i = 0; i < GetEntityRegistryInstance()->next_component_type; ++i)
        {
            ComponentData& data = GetEntityRegistryInstance()->component_data[i];
            RemovePoolElement(&data.pool, entity);
        }
        
        --GetEntityRegistryInstance()->entity_count;
        GetEntityRegistryInstance()->signatures[entity].set(0, false);

        for (auto& [signature, group] : GetEntityRegistryInstance()->entity_groups)
        {
            group.entities.erase(entity);
        }
    }

    bool IsEntityValid(const Entity entity)
    {
        return entity < MAX_ENTITIES && GetEntityRegistryInstance()->signatures[entity].test(0);
    }

    void EntitySignatureChanged(Entity entity, EntitySignature new_entity_signature)
    {
        for (auto& [signature, group] : GetEntityRegistryInstance()->entity_groups)
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
        NIT_CHECK_MSG(!GetEntityRegistryInstance()->entity_count, "Create the group before any entity gets created!");
        EntitySignature group_signature = BuildEntitySignature(types);

        if (GetEntityRegistryInstance()->entity_groups.count(group_signature) != 0)
        {
            return group_signature;
        }
        
        EntityGroup* group = &GetEntityRegistryInstance()->entity_groups[group_signature];
        group->signature = group_signature;
        return group_signature;
    }

    EntitySignature BuildEntitySignature(const Array<const char*>& types)
    {
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
        return GetEntityRegistryInstance()->entity_groups[signature];
    }
}
