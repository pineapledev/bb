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
    
    ComponentData* FindComponentDataByName(EntityRegistry& reg, const char* name);

    template<typename T>
    ComponentData* GetComponentData(EntityRegistry& reg)
    {
        const char* type_name = typeid(T).name();
        return FindComponentDataByName(reg, type_name);
    }
    
    template<typename T>
    void RegisterComponentType(EntityRegistry& reg)
    {
        NIT_CHECK_MSG(!GetComponentData<T>(reg), "Component type already registered!");
        NIT_CHECK_MSG(reg.next_component_type <= MAX_COMPONENTS_TYPES, "Components out of range!");
        ComponentData& component_data = reg.component_data[reg.next_component_type - 1];
        component_data.name  = typeid(T).name();
        component_data.type  = reg.next_component_type;
        InitPool<T>(&component_data.pool, MAX_COMPONENTS_TYPES, false);
        ++reg.next_component_type;
    }

    template<typename T>
    ComponentType GetComponentType(EntityRegistry& reg)
    {
        ComponentData* data = GetComponentData<T>(reg);
        NIT_CHECK_MSG(data, "Component type is not registered!");
        return data->type;
    }
    
    void InitEntityRegistry(EntityRegistry& reg);
    void FinishEntityRegistry(EntityRegistry& reg);
    Entity CreateEntity(EntityRegistry& reg);
    void DestroyEntity(EntityRegistry& reg, Entity entity);
    bool IsEntityValid(const EntityRegistry& reg, Entity entity);

    void EntitySignatureChanged(EntityRegistry& reg, Entity entity, EntitySignature new_entity_signature);
    
    template<typename T>
    T& AddComponent(EntityRegistry& reg, Entity entity)
    {
        NIT_CHECK_MSG(IsEntityValid(reg, entity), "Invalid entity!");
        NIT_CHECK_MSG(reg.signatures[entity].size() <= MAX_COMPONENTS_PER_ENTITY + 1, "Components per entity out of range!");
        ComponentData* component_data = GetComponentData<T>(reg);
        NIT_CHECK_MSG(component_data, "Invalid component type!");
        T data;
        InsertPoolElementWithID(&component_data->pool, entity, data);
        T& element = GetPoolElement<T>(&component_data->pool, entity);
        EntitySignature& signature = reg.signatures[entity]; 
        signature.set(GetComponentType<T>(reg), true);
        EntitySignatureChanged(reg, entity, signature);
        return element;
    }

    template<typename T>
    void RemoveComponent(EntityRegistry& reg, Entity entity)
    {
        NIT_CHECK_MSG(IsEntityValid(reg, entity), "Invalid entity!");
        NIT_CHECK_MSG(entity < MAX_ENTITIES, "Entity out of range!");
        ComponentData* component_data = GetComponentData<T>(reg);
        NIT_CHECK_MSG(component_data, "Invalid component type!");
        RemovePoolElement(&component_data->pool, entity);
        EntitySignature& signature = reg.signatures[entity]; 
        signature.set(GetComponentType<T>(), false);
        EntitySignatureChanged(reg, entity, signature);
    }

    template<typename T>
    T& GetComponent(EntityRegistry& reg, Entity entity)
    {
        NIT_CHECK_MSG(IsEntityValid(reg, entity), "Invalid entity!");
        ComponentData* component_data = GetComponentData<T>(reg);
        NIT_CHECK_MSG(component_data, "Invalid component type!");
        return GetPoolElement<T>(&component_data->pool, entity);
    }

    template<typename T>
    bool HasComponent(EntityRegistry& reg, Entity entity)
    {
        NIT_CHECK_MSG(IsEntityValid(reg, entity), "Invalid entity!");
        return reg.signatures[entity].test(GetComponentType<T>());
    }

    EntitySignature BuildEntitySignature(EntityRegistry& reg, const Array<const char*>& types);

    template <typename... T>
    EntitySignature BuildEntitySignature(EntityRegistry& reg)
    {
        Array<const char*> type_names = { (typeid(T).name())... };
        return BuildEntitySignature(reg, type_names);
    }
    
    EntitySignature CreateEntityGroup(EntityRegistry& reg, const Array<const char*>& types);

    template <typename... T>
    EntitySignature CreateEntityGroup(EntityRegistry& reg)
    {
        Array<const char*> type_names = { (typeid(T).name())... };
        return CreateEntityGroup(reg, type_names);
    }

    EntityGroup& GetEntityGroup(EntityRegistry& reg, EntitySignature signature);

    template <typename... T>
    EntityGroup& GetEntityGroup(EntityRegistry& reg)
    {
        Array<const char*> type_names = { (typeid(T).name())... };
        return GetEntityGroup(reg, BuildEntitySignature(reg, type_names));
    }
}