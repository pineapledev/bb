#include "entity.h"

namespace Nit
{
    ComponentData* FindComponentDataByName(EntityRegistry& reg, const char* name)
    {
        for (u32 i = 0; i < reg.next_component_type; ++i)
        {
            ComponentData& data = reg.component_data[i];
            if (data.name == name)
            {
                return &data;
            }
        }
        return nullptr;
    }

    void InitEntityRegistry(EntityRegistry& reg)
    {
        for (u32 i = 0; i < MAX_ENTITIES; ++i)
        {
            reg.available_entities.push(i);
        }
    }

    void FinishEntityRegistry(EntityRegistry& reg)
    {
        for (u32 i = 0; i < reg.next_component_type; ++i)
        {
            ComponentData& data = reg.component_data[i];
            free(data.array->components);
            data.array->components = nullptr;
            delete[] data.array;
            data.array = nullptr;
        }
    }

    TEntity CreateEntity(EntityRegistry& reg)
    {
        NIT_CHECK_MSG(reg.entity_count < MAX_ENTITIES, "Entity limit reached!");
        TEntity entity = reg.available_entities.front();
        reg.available_entities.pop();
        ++reg.entity_count;
        reg.signatures[entity].set(0, true);
        return entity;
    }

    void DestroyEntity(EntityRegistry& reg, TEntity entity)
    {
        NIT_CHECK_MSG(IsEntityValid(reg, entity), "Entity is not valid!");
        reg.signatures[entity].reset();
        reg.available_entities.push(entity);

        for (u32 i = 0; i < reg.next_component_type; ++i)
        {
            ComponentData& data = reg.component_data[i];
            data.array->remove_component_func(*data.array, entity);
        }
        
        --reg.entity_count;
        reg.signatures[entity].set(0, false);

        for (auto& [signature, group] : reg.entity_groups)
        {
            group.entities.erase(entity);
        }
    }

    bool IsEntityValid(const EntityRegistry& reg, TEntity entity)
    {
        return entity < MAX_ENTITIES && reg.signatures[entity].test(0);
    }

    void EntitySignatureChanged(EntityRegistry& reg, TEntity entity, TEntitySignature new_entity_signature)
    {
        for (auto& [signature, group] : reg.entity_groups)
        {
            if ((signature | new_entity_signature) == new_entity_signature)
            {
                group.entities.insert(entity);
                continue;
            }

            group.entities.erase(entity);
        }
    }

    TEntitySignature CreateEntityGroup(EntityRegistry& reg, const TArray<const char*>& types)
    {
        NIT_CHECK_MSG(!reg.entity_count, "Create the group before any entity gets created!");
        TEntitySignature group_signature = BuildEntitySignature(reg, types);

        if (reg.entity_groups.count(group_signature) != 0)
        {
            return group_signature;
        }
        
        EntityGroup* group = &reg.entity_groups[group_signature];
        group->signature = group_signature;
        return group_signature;
    }

    TEntitySignature BuildEntitySignature(EntityRegistry& reg, const TArray<const char*>& types)
    {
        TEntitySignature group_signature;
        group_signature.set(0, true);
        for (const char* type_name : types)
        {
            if (ComponentData* data = FindComponentDataByName(reg, type_name))
            {
                group_signature.set(data->type, true);
            }
        }
        return group_signature;
    }

    EntityGroup& GetEntityGroup(EntityRegistry& reg, TEntitySignature signature)
    {
        return reg.entity_groups[signature];
    }
}