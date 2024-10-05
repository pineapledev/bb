#pragma once

namespace Nit
{
    inline constexpr u32 MAX_ENTITIES = 3000;
    inline constexpr u32 MAX_COMPONENTS_PER_ENTITY = 30;
    inline constexpr u32 MAX_COMPONENTS_TYPES = 100;
    inline constexpr u32 INVALID_INDEX = U32_MAX;
    
    using Entity        = u32;

    // First bit of the signature would be used to know if the entity is valid or not
    using EntitySignature = Bitset<MAX_COMPONENTS_PER_ENTITY + 1>;
    
    struct ComponentPool
    {
        FastPool data_pool;
        u8       type_index  = 0;
    };

    struct EntityGroup
    {
        EntitySignature signature;
        Set<Entity>     entities;
    };

    struct EntityRegistry
    {
        Queue<Entity>                     available_entities;
        EntitySignature                   signatures[MAX_ENTITIES];
        u32                               entity_count = 0;
        Map<EntitySignature, EntityGroup> entity_groups;
        ComponentPool                     component_pool[MAX_COMPONENTS_TYPES];
        u8                                next_component_type_index = 1;
    };

    void SetEntityRegistryInstance(EntityRegistry* entity_registry_instance);
    
    EntityRegistry* GetEntityRegistryInstance();
    
    ComponentPool* FindComponentPool(const Type* type);
    
    template<typename T>
    ComponentPool* FindComponentPool()
    {
        return FindComponentPool(GetType<T>());
    }
    
    template<typename T>
    void RegisterComponentType()
    {
        EntityRegistry* entity_registry = GetEntityRegistryInstance(); 
        NIT_CHECK_MSG(entity_registry->next_component_type_index <= MAX_COMPONENTS_TYPES, "Components out of range!");

        if (!IsTypeRegistered<T>())
        {
            RegisterType<T>();
        }
        
        ComponentPool& component_pool  = entity_registry->component_pool[entity_registry->next_component_type_index - 1];
        component_pool.data_pool.type  = GetType<T>();
        component_pool.type_index      = entity_registry->next_component_type_index;
        
        InitPool<T>(&component_pool.data_pool, MAX_COMPONENTS_TYPES, false);
        ++entity_registry->next_component_type_index;
    }

    template<typename T>
    u8 GetComponentTypeIndex()
    {
        ComponentPool* component_pool = FindComponentPool<T>();
        NIT_CHECK_MSG(component_pool, "Component type is not registered!");
        return component_pool->type_index;
    }
    
    void InitEntityRegistry();
    void FinishEntityRegistry();
    Entity CreateEntity();
    void DestroyEntity(Entity entity);
    bool IsEntityValid(Entity entity);

    void EntitySignatureChanged(Entity entity, EntitySignature new_entity_signature);
    
    template<typename T>
    T& AddComponent(Entity entity)
    {
        NIT_CHECK_MSG(IsEntityValid(entity), "Invalid entity!");
        NIT_CHECK_MSG(GetEntityRegistryInstance()->signatures[entity].size() <= MAX_COMPONENTS_PER_ENTITY + 1, "Components per entity out of range!");
        ComponentPool* component_pool = FindComponentPool<T>();
        NIT_CHECK_MSG(component_pool, "Invalid component type!");
        T data;
        InsertPoolElementWithID(&component_pool->data_pool, entity, data);
        T& element = GetPoolElement<T>(&component_pool->data_pool, entity);
        EntitySignature& signature = GetEntityRegistryInstance()->signatures[entity]; 
        signature.set(GetComponentTypeIndex<T>(), true);
        EntitySignatureChanged(entity, signature);
        return element;
    }

    template<typename T>
    void RemoveComponent(Entity entity)
    {
        NIT_CHECK_MSG(IsEntityValid(entity), "Invalid entity!");
        NIT_CHECK_MSG(entity < MAX_ENTITIES, "Entity out of range!");
        ComponentPool* component_pool = FindComponentPool<T>();
        NIT_CHECK_MSG(component_pool, "Invalid component type!");
        RemovePoolElement(&component_pool->data_pool, entity);
        EntitySignature& signature = GetEntityRegistryInstance()->signatures[entity]; 
        signature.set(GetComponentTypeIndex<T>(), false);
        EntitySignatureChanged(entity, signature);
    }

    template<typename T>
    T& GetComponent(Entity entity)
    {
        NIT_CHECK_MSG(IsEntityValid(entity), "Invalid entity!");
        ComponentPool* component_pool = FindComponentPool<T>();
        NIT_CHECK_MSG(component_pool, "Invalid component type!");
        return GetPoolElement<T>(&component_pool->data_pool, entity);
    }

    template<typename T>
    bool HasComponent(Entity entity)
    {
        NIT_CHECK_MSG(IsEntityValid(entity), "Invalid entity!");
        return GetEntityRegistryInstance()->signatures[entity].test(GetComponentTypeIndex<T>());
    }

    EntitySignature BuildEntitySignature(const Array<u64>& type_hashes);

    template <typename... T>
    EntitySignature BuildEntitySignature()
    {
        Array<u64> type_hashes = { GetTypeHash<T>()... };
        return BuildEntitySignature(type_hashes);
    }
    
    EntitySignature CreateEntityGroup(const Array<u64>& type_hashes);
    
    template <typename... T>
    EntitySignature CreateEntityGroup()
    {
        Array<u64> type_hashes = { GetTypeHash<T>()... };
        return CreateEntityGroup(type_hashes);
    }

    EntityGroup& GetEntityGroup(EntitySignature signature);

    template <typename... T>
    EntityGroup& GetEntityGroup()
    {
        Array<u64> type_hashes = { GetTypeHash<T>()... };
        return GetEntityGroup(BuildEntitySignature(type_hashes));
    }
}
