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
        u8                      type_index  = 0;
        FastPool                data_pool;
        Delegate<void(Entity)>  fn_add_to_entity;
        Delegate<void(Entity)>  fn_remove_from_entity;
        Delegate<bool(Entity)>  fn_is_in_entity;
        Delegate<void*(Entity)> fn_get_from_entity;
    };
    
    struct EntityGroup
    {
        EntitySignature signature;
        Set<Entity>     entities;
    };

    struct ComponentAddedArgs
    {
        Entity entity = 0;
        Type*  type   = nullptr;
    };

    struct ComponentRemovedArgs
    {
        Entity entity = 0;
        Type*  type   = nullptr;
    };

    using ComponentAddedEvent   = Event<const ComponentAddedArgs&>;
    using ComponentRemovedEvent = Event<const ComponentRemovedArgs&>;
    
    struct EntityRegistry
    {
        Queue<Entity>                     available_entities;
        EntitySignature                   signatures[MAX_ENTITIES];
        u32                               entity_count = 0;
        Map<EntitySignature, EntityGroup> entity_groups;
        ComponentPool                     component_pool[MAX_COMPONENTS_TYPES];
        u8                                next_component_type_index = 1;
        ComponentAddedEvent               component_added_event;
        ComponentRemovedEvent             component_removed_event;
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
    T& AddComponentSilent(Entity entity, const T& data = {});
    
    template<typename T>
    void RemoveComponent(Entity entity);
    
    template<typename T>
    bool HasComponent(Entity entity);

    template<typename T>
    T* GetComponentPtr(Entity entity);
    
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
        
        Bind(component_pool.fn_add_to_entity, {[](Entity entity) {
            AddComponentSilent<T>(entity);  
        }});
        
        Bind(component_pool.fn_remove_from_entity, {[](Entity entity){
            RemoveComponent<T>(entity);
        }});
        
        Bind(component_pool.fn_is_in_entity, {[](Entity entity){
            return HasComponent<T>(entity);
        }});

        Bind(component_pool.fn_get_from_entity, {[](Entity entity) -> void* {
            return GetComponentPtr<T>(entity);
        }});
        
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
    T& AddComponentSilent(Entity entity, const T& data)
    {
        NIT_CHECK_MSG(IsEntityValid(entity), "Invalid entity!");
        NIT_CHECK_MSG(GetEntityRegistryInstance()->signatures[entity].size() <= MAX_COMPONENTS_PER_ENTITY + 1, "Components per entity out of range!");
        ComponentPool* component_pool = FindComponentPool<T>();
        NIT_CHECK_MSG(component_pool, "Invalid component type!");
        T& element = InsertPoolElementWithID(&component_pool->data_pool, entity, data);
        EntitySignature& signature = GetEntityRegistryInstance()->signatures[entity]; 
        signature.set(GetComponentTypeIndex<T>(), true);
        EntitySignatureChanged(entity, signature);
        return element;
    }
    
    template<typename T>
    T& AddComponent(Entity entity, const T& data)
    {
        NIT_CHECK_MSG(IsEntityValid(entity), "Invalid entity!");
        NIT_CHECK_MSG(GetEntityRegistryInstance()->signatures[entity].size() <= MAX_COMPONENTS_PER_ENTITY + 1, "Components per entity out of range!");
        ComponentPool* component_pool = FindComponentPool<T>();
        NIT_CHECK_MSG(component_pool, "Invalid component type!");
        T& element = InsertPoolElementWithID(&component_pool->data_pool, entity, data);
        EntitySignature& signature = GetEntityRegistryInstance()->signatures[entity]; 
        signature.set(GetComponentTypeIndex<T>(), true);
        EntitySignatureChanged(entity, signature);
        ComponentAddedArgs args;
        args.entity = entity;
        args.type = component_pool->data_pool.type;
        Broadcast<const ComponentAddedArgs&>(GetEntityRegistryInstance()->component_added_event, args);
        return element;
    }

    template<typename T>
    void RemoveComponent(Entity entity)
    {
        NIT_CHECK_MSG(IsEntityValid(entity), "Invalid entity!");
        NIT_CHECK_MSG(entity < MAX_ENTITIES, "Entity out of range!");
        ComponentPool* component_pool = FindComponentPool<T>();
        NIT_CHECK_MSG(component_pool, "Invalid component type!");

        ComponentRemovedArgs args;
        args.entity = entity;
        args.type = component_pool->data_pool.type;
        Broadcast<const ComponentRemovedArgs&>(GetEntityRegistryInstance()->component_removed_event, args);
        
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
    T* GetComponentPtr(Entity entity)
    {
        NIT_CHECK_MSG(IsEntityValid(entity), "Invalid entity!");
        ComponentPool* component_pool = FindComponentPool<T>();
        NIT_CHECK_MSG(component_pool, "Invalid component type!");
        return GetPoolElementPtr<T>(&component_pool->data_pool, entity);
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
    
    void SerializeEntity(Entity entity, YAML::Emitter& emitter);
    
    Entity DeserializeEntity(const YAML::Node& node);
}
