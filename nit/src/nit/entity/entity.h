#pragma once

#ifndef NIT_MAX_COMPONENT_TYPES
    #define NIT_MAX_COMPONENT_TYPES 100
#endif

namespace nit
{
    inline constexpr u32 NULL_ENTITY = U32_MAX;
    
    using EntityID = u32;

    // First bit of the signature would be used to know if the entity is valid or not
    using EntitySignature = Bitset<NIT_MAX_COMPONENT_TYPES + 1>;
    
    struct EntityData
    {
        EntityID         id             = NULL_ENTITY;
        bool             enabled        = true;
        bool             global_enabled = true;
        EntitySignature  signature      = {};
        String           name;
        UUID             uuid           = {0};
        EntityID         parent         = NULL_ENTITY;
        Array<EntityID>  children       = {};
    };
    
    struct ComponentPool
    {
        u32                           type_index  = 0;
        Pool                          data_pool;
        
        Delegate<void(EntityID, void*, bool)> fn_add_to_entity;
        Delegate<void(EntityID)>              fn_remove_from_entity;
        Delegate<bool(EntityID)>              fn_is_in_entity;
        Delegate<void*(EntityID)>             fn_get_from_entity;
    };
    
    struct EntityGroup
    {
        EntitySignature   signature;
        Set<EntityID>     entities;
    };

    struct ComponentAddedArgs
    {
        EntityID entity = 0;
        Type*  type   = nullptr;
    };

    struct ComponentRemovedArgs
    {
        EntityID entity = 0;
        Type*  type   = nullptr;
    };
    
    using ComponentAddedListener   = Listener<const ComponentAddedArgs&>; 
    using ComponentRemovedListener = Listener<const ComponentRemovedArgs&>; 
    using ComponentAddedEvent      = Event<const ComponentAddedArgs&>;
    using ComponentRemovedEvent    = Event<const ComponentRemovedArgs&>;
    
    struct EntityRegistry
    {
        Pool                              entities;
        u32                               entity_count = 0;
        Map<EntitySignature, EntityGroup> entity_groups;
        Map<String, Array<u64>>           entity_presets;
        ComponentPool*                    component_pool;
        u32                               next_component_type_index = 1;
        ComponentAddedEvent               component_added_event;
        ComponentRemovedEvent             component_removed_event;
        u32                               max_entities = 100000;
    };

    void            entity_registry_set_instance(EntityRegistry* entity_registry_instance);
    EntityRegistry* entity_registry_get_instance();
    
    
    template<typename T>
    void entity_remove(EntityID entity);

    template<typename T>
    T* entity_get_ptr(EntityID entity);
        
    template<typename T>
    bool entity_has(EntityID entity);

    template<typename T>
    T& entity_add(EntityID entity, const T& data = {}, bool invoke_add_event = true);
    
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
        component_pool.type_index      = entity_registry->next_component_type_index;

        void (*fn_add_to_entity)(EntityID, void*, bool) = [](EntityID entity, void* data, bool invoke_add_event) {

            if (!data)
            {
                entity_add<T>(entity, T{}, invoke_add_event);  
                return;
            }
            
            T* casted_data = static_cast<T*>(data);
            entity_add<T>(entity, *casted_data, invoke_add_event);
        };

        void (*fn_remove_from_entity)(EntityID) = [](EntityID entity){
            entity_remove<T>(entity);
        };

        bool (*fn_is_in_entity)(EntityID) = [](EntityID entity){
            return entity_has<T>(entity);
        };

        void* (*fn_get_from_entity)(EntityID) = [](EntityID entity) -> void* {
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
    EntityID        entity_create(const String& name = "");
    void            entity_destroy(EntityID entity);
    bool            entity_valid(EntityID entity);
    EntitySignature entity_get_signature(EntityID entity);
    void            entity_signature_changed(EntityID entity, EntitySignature new_entity_signature);
    ComponentPool*  entity_find_component_pool(const Type* type);
    EntityID        entity_clone(EntityID entity, const Vector3& position = V3_ZERO);
    
    template<typename T>
    ComponentPool* entity_find_component_pool()
    {
        return entity_find_component_pool(type_get<T>());
    }

    EntitySignature entity_build_signature(const Array<u64>& type_hashes);

    template <typename... T>
    EntitySignature entity_build_signature()
    {
        Array<u64> type_hashes = { get_type_hash<T>()... };
        return entity_build_signature(type_hashes);
    }

    template <typename T>
    T& entity_add(EntityID entity, const T& data, bool invoke_add_event)
    {
        NIT_CHECK_MSG(entity_valid(entity), "Invalid entity!");
        NIT_CHECK_MSG(pool_get_data<EntityData>(&entity_registry_get_instance()->entities, entity)->signature.size() <= NIT_MAX_COMPONENT_TYPES + 1, "Components per entity out of range!");
        ComponentPool* component_pool = entity_find_component_pool<T>();
        NIT_CHECK_MSG(component_pool, "Invalid component type!");
        T* element = pool_insert_data_with_id(&component_pool->data_pool, entity, data);
        EntitySignature& signature = pool_get_data<EntityData>(&entity_registry_get_instance()->entities, entity)->signature;
        signature.set(entity_component_type_index<T>(), true);
        entity_signature_changed(entity, signature);
        ComponentAddedArgs args;
        args.entity = entity;
        args.type = component_pool->data_pool.type;

        if (invoke_add_event)
        {
            event_broadcast<const ComponentAddedArgs&>(entity_registry_get_instance()->component_added_event, args);
        }
        return *element;
    }

    template <typename T>
    void entity_remove(EntityID entity)
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
        EntitySignature& signature = pool_get_data<EntityData>(&entity_registry_get_instance()->entities, entity)->signature;
        signature.set(entity_component_type_index<T>(), false);
        entity_signature_changed(entity, signature);
    }

    template <typename T>
    T& entity_get(EntityID entity)
    {
        NIT_CHECK_MSG(entity_valid(entity), "Invalid entity!");
        ComponentPool* component_pool = entity_find_component_pool<T>();
        NIT_CHECK_MSG(component_pool, "Invalid component type!");
        return *pool_get_data<T>(&component_pool->data_pool, entity);
    }
    
    bool          entity_enabled(EntityID entity);
    bool          entity_global_enabled(EntityID entity);
    void          entity_set_enabled(EntityID entity, bool enabled = true);
    const String& entity_get_name(EntityID entity);
    void          entity_set_name(EntityID entity, const String& name);
    UUID          entity_get_uuid(EntityID entity);
    void          entity_add_child(EntityID entity, EntityID child);
    void          entity_get_children(EntityID entity, Array<EntityID>& children);
    void          entity_set_parent(EntityID entity, EntityID parent);
    EntityID      entity_get_parent(EntityID entity);
    void          entity_remove_child(EntityID entity, EntityID child);

    struct EntityArray { EntityData* entities = nullptr; u32 count = 0; };
    
    EntityArray entity_get_alive_entities();
    
    template <typename T>
    T* entity_get_ptr(EntityID entity)
    {
        NIT_CHECK_MSG(entity_valid(entity), "Invalid entity!");
        ComponentPool* component_pool = entity_find_component_pool<T>();
        NIT_CHECK_MSG(component_pool, "Invalid component type!");
        return pool_get_data<T>(&component_pool->data_pool, entity);
    }

    template <typename T>
    bool entity_has(EntityID entity)
    {
        NIT_CHECK_MSG(entity_valid(entity), "Invalid entity!");
        return pool_get_data<EntityData>(&entity_registry_get_instance()->entities, entity)->signature.test(entity_component_type_index<T>());
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

    void entity_create_preset(const String& name, const Array<u64>& type_hashes);

    template <typename... T>
    void entity_create_preset(const String& name)
    {
        Array<u64> type_hashes = {get_type_hash<T>()...};
        entity_create_preset(name, type_hashes);
    }

    EntityID entity_create_from_preset(const String& name);

    void     entity_serialize(EntityID entity, YAML::Emitter& emitter);
    EntityID entity_deserialize(const YAML::Node& node, EntityID parent = NULL_ENTITY);
}
