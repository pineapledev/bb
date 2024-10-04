#pragma once

namespace Nit
{
    inline constexpr u32 MAX_ENTITIES = 3000;
    inline constexpr u32 MAX_COMPONENTS_PER_ENTITY = 30;
    inline constexpr u32 MAX_COMPONENTS_TYPES = 100;
    inline constexpr u32 INVALID_INDEX = U32_MAX;
    
    using Entity        = u32;
    using ComponentType = u8;

    // First bit of the signature would be used to know if the entity is valid or not
    using EntitySignature = Bitset<MAX_COMPONENTS_PER_ENTITY + 1>;
    
    struct ComponentData
    {
        const char*     name  = "";
        ComponentType   type  = 0;
        FastPool        pool;
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
        ComponentData                     component_data[MAX_COMPONENTS_TYPES];
        ComponentType                     next_component_type = 1;
    };

    void SetEntityRegistryInstance(EntityRegistry* entity_registry_instance);
    EntityRegistry* GetEntityRegistryInstance();
    
    ComponentData* FindComponentDataByName(const char* name);

    template<typename T>
    ComponentData* GetComponentData()
    {
        const char* type_name = typeid(T).name();
        return FindComponentDataByName(type_name);
    }
    
    template<typename T>
    void RegisterComponentType()
    {
        NIT_CHECK_MSG(!GetComponentData<T>(), "Component type already registered!");
        NIT_CHECK_MSG(GetEntityRegistryInstance()->next_component_type <= MAX_COMPONENTS_TYPES, "Components out of range!");
        ComponentData& component_data = GetEntityRegistryInstance()->component_data[GetEntityRegistryInstance()->next_component_type - 1];
        component_data.name  = typeid(T).name();
        component_data.type  = GetEntityRegistryInstance()->next_component_type;
        InitPool<T>(&component_data.pool, MAX_COMPONENTS_TYPES, false);
        ++GetEntityRegistryInstance()->next_component_type;
    }

    template<typename T>
    ComponentType GetComponentType()
    {
        ComponentData* data = GetComponentData<T>();
        NIT_CHECK_MSG(data, "Component type is not registered!");
        return data->type;
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
        ComponentData* component_data = GetComponentData<T>();
        NIT_CHECK_MSG(component_data, "Invalid component type!");
        T data;
        InsertPoolElementWithID(&component_data->pool, entity, data);
        T& element = GetPoolElement<T>(&component_data->pool, entity);
        EntitySignature& signature = GetEntityRegistryInstance()->signatures[entity]; 
        signature.set(GetComponentType<T>(), true);
        EntitySignatureChanged(entity, signature);
        return element;
    }

    template<typename T>
    void RemoveComponent(Entity entity)
    {
        NIT_CHECK_MSG(IsEntityValid(entity), "Invalid entity!");
        NIT_CHECK_MSG(entity < MAX_ENTITIES, "Entity out of range!");
        ComponentData* component_data = GetComponentData<T>();
        NIT_CHECK_MSG(component_data, "Invalid component type!");
        RemovePoolElement(&component_data->pool, entity);
        EntitySignature& signature = GetEntityRegistryInstance()->signatures[entity]; 
        signature.set(GetComponentType<T>(), false);
        EntitySignatureChanged(entity, signature);
    }

    template<typename T>
    T& GetComponent(Entity entity)
    {
        NIT_CHECK_MSG(IsEntityValid(entity), "Invalid entity!");
        ComponentData* component_data = GetComponentData<T>();
        NIT_CHECK_MSG(component_data, "Invalid component type!");
        return GetPoolElement<T>(&component_data->pool, entity);
    }

    template<typename T>
    bool HasComponent(Entity entity)
    {
        NIT_CHECK_MSG(IsEntityValid(entity), "Invalid entity!");
        return GetEntityRegistryInstance()->signatures[entity].test(GetComponentType<T>());
    }

    EntitySignature BuildEntitySignature(const Array<const char*>& types);

    template <typename... T>
    EntitySignature BuildEntitySignature()
    {
        Array<const char*> type_names = { (typeid(T).name())... };
        return BuildEntitySignature(type_names);
    }
    
    EntitySignature CreateEntityGroup(const Array<const char*>& types);

    template <typename... T>
    EntitySignature CreateEntityGroup()
    {
        Array<const char*> type_names = { (typeid(T).name())... };
        return CreateEntityGroup(type_names);
    }

    EntityGroup& GetEntityGroup(EntitySignature signature);

    template <typename... T>
    EntityGroup& GetEntityGroup()
    {
        Array<const char*> type_names = { (typeid(T).name())... };
        return GetEntityGroup(BuildEntitySignature(type_names));
    }
}