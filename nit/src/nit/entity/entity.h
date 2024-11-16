#pragma once

#ifndef NIT_MAX_COMPONENT_TYPES
    #define NIT_MAX_COMPONENT_TYPES 100
#endif

namespace nit
{
    inline constexpr u32 NULL_ENTITY = U32_MAX;
    
    using Entity = u32;

    // First bit of the signature would be used to know if the entity is valid or not
    using EntitySignature = Bitset<NIT_MAX_COMPONENT_TYPES + 1>;

    struct ComponentPool
    {
        u32                           type_index  = 0;
        Pool                          data_pool;
        
        Delegate<void(Entity, void*)> fn_add_to_entity;
        Delegate<void(Entity)>        fn_remove_from_entity;
        Delegate<bool(Entity)>        fn_is_in_entity;
        Delegate<void*(Entity)>       fn_get_from_entity;
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

    void            entity_registry_set_instance(EntityRegistry* entity_registry_instance);
    EntityRegistry* entity_registry_get_instance();
    
    

    template<typename T>
    T& component_add_silent(Entity entity, const T& data = {});
    
    template<typename T>
    void entity_remove(Entity entity);

    template<typename T>
    T* entity_get_ptr(Entity entity);
        
    template<typename T>
    bool entity_has(Entity entity);
    
    template<typename T>
    void component_register(const TypeArgs<T>& args = {})
    {
        EntityRegistry* entity_registry = entity_registry_get_instance(); 
        NIT_CHECK_MSG(entity_registry->next_component_type_index <= NIT_MAX_COMPONENT_TYPES, "Components out of range!");

        if (!type_exists<T>())
        {
            type_register<T>(args);
        }
        
        ComponentPool& component_pool  = entity_registry->component_pool[entity_registry->next_component_type_index - 1];
        component_pool.data_pool.type  = GetType<T>();
        component_pool.type_index      = entity_registry->next_component_type_index;

        void (*fn_add_to_entity)(Entity, void*) = [](Entity entity, void* data) {

            if (!data)
            {
                component_add_silent<T>(entity);  
                return;
            }

            T* casted_data = static_cast<T*>(data);
            component_add_silent(entity, *casted_data);
        };

        void (*fn_remove_from_entity)(Entity) = [](Entity entity){
            entity_remove<T>(entity);
        };

        bool (*fn_is_in_entity)(Entity) = [](Entity entity){
            return entity_has<T>(entity);
        };

        void* (*fn_get_from_entity)(Entity) = [](Entity entity) -> void* {
            return entity_get_ptr<T>(entity);
        };
        
        delegate_bind(component_pool.fn_add_to_entity, fn_add_to_entity);
        delegate_bind(component_pool.fn_remove_from_entity, fn_remove_from_entity);
        delegate_bind(component_pool.fn_is_in_entity, fn_is_in_entity);
        delegate_bind(component_pool.fn_get_from_entity, fn_get_from_entity);
        
        pool_load<T>(&component_pool.data_pool, entity_registry->max_entities, false);
        ++entity_registry->next_component_type_index;
    }

    template<typename T>
    u32 entity_component_type_index()
    {
        ComponentPool* component_pool = entity_find_component_pool<T>();
        NIT_CHECK_MSG(component_pool, "Component type is not registered!");
        return component_pool->type_index;
    }
    
    void            entity_registry_init();
    void            entity_registry_finish();
    Entity          entity_create();
    void            entity_destroy(Entity entity);
    bool            entity_valid(Entity entity);
    EntitySignature entity_get_signature(Entity entity);
    void            entity_signature_changed(Entity entity, EntitySignature new_entity_signature);
    ComponentPool*  entity_find_component_pool(const Type* type);
    Entity          entity_clone(Entity entity);
    
    template<typename T>
    ComponentPool* entity_find_component_pool()
    {
        return entity_find_component_pool(GetType<T>());
    }
    
    template<typename T>
    T& component_add_silent(Entity entity, const T& data)
    {
        NIT_CHECK_MSG(entity_valid(entity), "Invalid entity!");
        NIT_CHECK_MSG(entity_registry_get_instance()->signatures[entity].size() <= NIT_MAX_COMPONENT_TYPES + 1, "Components per entity out of range!");
        ComponentPool* component_pool = entity_find_component_pool<T>();
        NIT_CHECK_MSG(component_pool, "Invalid component type!");
        T* element = pool_insert_data_with_id(&component_pool->data_pool, entity, data);
        EntitySignature& signature = entity_registry_get_instance()->signatures[entity]; 
        signature.set(entity_component_type_index<T>(), true);
        entity_signature_changed(entity, signature);
        return *element;
    }

    EntitySignature entity_build_signature(const Array<u64>& type_hashes);

    template <typename... T>
    EntitySignature entity_build_signature()
    {
        Array<u64> type_hashes = { get_type_hash<T>()... };
        return entity_build_signature(type_hashes);
    }

    template <typename T>
    T& entity_add(Entity entity, const T& data = {})
    {
        NIT_CHECK_MSG(entity_valid(entity), "Invalid entity!");
        NIT_CHECK_MSG(entity_registry_get_instance()->signatures[entity].size() <= NIT_MAX_COMPONENT_TYPES + 1,
                      "Components per entity out of range!");
        ComponentPool* component_pool = entity_find_component_pool<T>();
        NIT_CHECK_MSG(component_pool, "Invalid component type!");
        T* element = pool_insert_data_with_id(&component_pool->data_pool, entity, data);
        EntitySignature& signature = entity_registry_get_instance()->signatures[entity];
        signature.set(entity_component_type_index<T>(), true);
        entity_signature_changed(entity, signature);
        ComponentAddedArgs args;
        args.entity = entity;
        args.type = component_pool->data_pool.type;
        event_broadcast<const ComponentAddedArgs&>(entity_registry_get_instance()->component_added_event, args);
        return *element;
    }

    template <typename T>
    void entity_remove(Entity entity)
    {
        NIT_CHECK_MSG(entity_valid(entity), "Invalid entity!");
        NIT_CHECK_MSG(entity < entity_registry_get_instance()->max_entities, "Entity out of range!");
        ComponentPool* component_pool = entity_find_component_pool<T>();
        NIT_CHECK_MSG(component_pool, "Invalid component type!");

        ComponentRemovedArgs args;
        args.entity = entity;
        args.type = component_pool->data_pool.type;
        event_broadcast<const ComponentRemovedArgs&>(entity_registry_get_instance()->component_removed_event, args);

        pool_delete_data(&component_pool->data_pool, entity);
        EntitySignature& signature = entity_registry_get_instance()->signatures[entity];
        signature.set(entity_component_type_index<T>(), false);
        entity_signature_changed(entity, signature);
    }

    template <typename T>
    T& entity_get(Entity entity)
    {
        NIT_CHECK_MSG(entity_valid(entity), "Invalid entity!");
        ComponentPool* component_pool = entity_find_component_pool<T>();
        NIT_CHECK_MSG(component_pool, "Invalid component type!");
        return *pool_get_data<T>(&component_pool->data_pool, entity);
    }

    template <typename T>
    T* entity_get_ptr(Entity entity)
    {
        NIT_CHECK_MSG(entity_valid(entity), "Invalid entity!");
        ComponentPool* component_pool = entity_find_component_pool<T>();
        NIT_CHECK_MSG(component_pool, "Invalid component type!");
        return pool_get_data<T>(&component_pool->data_pool, entity);
    }

    template <typename T>
    bool entity_has(Entity entity)
    {
        NIT_CHECK_MSG(entity_valid(entity), "Invalid entity!");
        return entity_registry_get_instance()->signatures[entity].test(entity_component_type_index<T>());
    }

    EntityGroup& entity_get_group(EntitySignature signature);

    template <typename... T>
    EntityGroup& entity_get_group()
    {
        Array<u64> type_hashes = {get_type_hash<T>()...};
        return entity_get_group(entity_build_signature(type_hashes));
    }

    EntitySignature entity_create_group(const Array<u64>& type_hashes);

    template <typename... T>
    EntitySignature entity_create_group()
    {
        Array<u64> type_hashes = {get_type_hash<T>()...};
        return entity_create_group(type_hashes);
    }

    void   entity_serialize(Entity entity, YAML::Emitter& emitter);
    Entity entity_deserialize(const YAML::Node& node);
}
