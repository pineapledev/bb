#pragma once

#ifndef NIT_MAX_COMPONENT_TYPES
    #define NIT_MAX_COMPONENT_TYPES 100
#endif

namespace Nit
{
    inline constexpr u32 NULL_ENTITY = U32_MAX;
    
    using Entity = u32;

    // First bit of the signature would be used to know if the entity is valid or not
    using EntitySignature = Bitset<NIT_MAX_COMPONENT_TYPES + 1>;

    struct ComponentPool
    {
        u32                      type_index  = 0;
        Pool                    data_pool;
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

    using ComponentAddedListener   = Listener<const ComponentAddedArgs&>; 
    using ComponentRemovedListener = Listener<const ComponentRemovedArgs&>; 
    using ComponentAddedEvent      = Event<const ComponentAddedArgs&>;
    using ComponentRemovedEvent    = Event<const ComponentRemovedArgs&>;
    
    struct EntityRegistry
    {
        Queue<Entity>                     available_entities;
        EntitySignature*                  signatures;
        u32                               entity_count = 0;
        Map<EntitySignature, EntityGroup> entity_groups;
        ComponentPool*                    component_pool;
        u32                               next_component_type_index = 1;
        ComponentAddedEvent               component_added_event;
        ComponentRemovedEvent             component_removed_event;
        u32                               max_entities = 100000;
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
    void RegisterComponentType(const TypeArgs<T>& args = {})
    {
        EntityRegistry* entity_registry = GetEntityRegistryInstance(); 
        NIT_CHECK_MSG(entity_registry->next_component_type_index <= NIT_MAX_COMPONENT_TYPES, "Components out of range!");

        if (!IsTypeRegistered<T>())
        {
            RegisterType<T>(args);
        }
        
        ComponentPool& component_pool  = entity_registry->component_pool[entity_registry->next_component_type_index - 1];
        component_pool.data_pool.type  = GetType<T>();
        component_pool.type_index      = entity_registry->next_component_type_index;

        void (*fn_add_to_entity)(Entity) = [](Entity entity) {
            AddComponentSilent<T>(entity);  
        };

        void (*fn_remove_from_entity)(Entity) = [](Entity entity){
            RemoveComponent<T>(entity);
        };

        bool (*fn_is_in_entity)(Entity) = [](Entity entity){
            return HasComponent<T>(entity);
        };

        void* (*fn_get_from_entity)(Entity) = [](Entity entity) -> void* {
            return GetComponentPtr<T>(entity);
        };
        
        Bind(component_pool.fn_add_to_entity, fn_add_to_entity);
        Bind(component_pool.fn_remove_from_entity, fn_remove_from_entity);
        Bind(component_pool.fn_is_in_entity, fn_is_in_entity);
        Bind(component_pool.fn_get_from_entity, fn_get_from_entity);
        
        Load<T>(&component_pool.data_pool, entity_registry->max_entities, false);
        ++entity_registry->next_component_type_index;
    }

    template<typename T>
    u32 GetComponentTypeIndex()
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
        NIT_CHECK_MSG(GetEntityRegistryInstance()->signatures[entity].size() <= NIT_MAX_COMPONENT_TYPES + 1, "Components per entity out of range!");
        ComponentPool* component_pool = FindComponentPool<T>();
        NIT_CHECK_MSG(component_pool, "Invalid component type!");
        T* element = InsertDataWithID(&component_pool->data_pool, entity, data);
        EntitySignature& signature = GetEntityRegistryInstance()->signatures[entity]; 
        signature.set(GetComponentTypeIndex<T>(), true);
        EntitySignatureChanged(entity, signature);
        return *element;
    }
    
    template<typename T>
    T& AddComponent(Entity entity, const T& data = {})
    {
        NIT_CHECK_MSG(IsEntityValid(entity), "Invalid entity!");
        NIT_CHECK_MSG(GetEntityRegistryInstance()->signatures[entity].size() <= NIT_MAX_COMPONENT_TYPES + 1, "Components per entity out of range!");
        ComponentPool* component_pool = FindComponentPool<T>();
        NIT_CHECK_MSG(component_pool, "Invalid component type!");
        T* element = InsertDataWithID(&component_pool->data_pool, entity, data);
        EntitySignature& signature = GetEntityRegistryInstance()->signatures[entity]; 
        signature.set(GetComponentTypeIndex<T>(), true);
        EntitySignatureChanged(entity, signature);
        ComponentAddedArgs args;
        args.entity = entity;
        args.type = component_pool->data_pool.type;
        Broadcast<const ComponentAddedArgs&>(GetEntityRegistryInstance()->component_added_event, args);
        return *element;
    }

    template<typename T>
    void RemoveComponent(Entity entity)
    {
        NIT_CHECK_MSG(IsEntityValid(entity), "Invalid entity!");
        NIT_CHECK_MSG(entity < GetEntityRegistryInstance()->max_entities, "Entity out of range!");
        ComponentPool* component_pool = FindComponentPool<T>();
        NIT_CHECK_MSG(component_pool, "Invalid component type!");

        ComponentRemovedArgs args;
        args.entity = entity;
        args.type = component_pool->data_pool.type;
        Broadcast<const ComponentRemovedArgs&>(GetEntityRegistryInstance()->component_removed_event, args);
        
        DeleteData(&component_pool->data_pool, entity);
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
        return *GetData<T>(&component_pool->data_pool, entity);
    }
    
    template<typename T>
    T* GetComponentPtr(Entity entity)
    {
        NIT_CHECK_MSG(IsEntityValid(entity), "Invalid entity!");
        ComponentPool* component_pool = FindComponentPool<T>();
        NIT_CHECK_MSG(component_pool, "Invalid component type!");
        return GetData<T>(&component_pool->data_pool, entity);
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
