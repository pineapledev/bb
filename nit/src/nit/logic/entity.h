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

    struct ComponentArray
    {
        void*   components = nullptr;
        u32     entity_to_index[MAX_ENTITIES];
        Entity index_to_entity[MAX_ENTITIES];
        u32     living_count = 0;
        Function<void(ComponentArray&, Entity)> remove_component_func;
    };
    
    template<typename T>
    T& InsertComponentData(ComponentArray& array, Entity entity, const T& component)
    {
        u32 living_count = array.living_count;
        NIT_CHECK_MSG(living_count < MAX_ENTITIES, "Component array out of bounds");
        NIT_CHECK_MSG(array.entity_to_index[entity] == INVALID_INDEX, "Component added to the same entity more than once");
        T* components = static_cast<T*>(array.components);
        
        components[living_count] = component;
        T& data = components[living_count];
        array.entity_to_index[entity] = living_count;
        array.index_to_entity[living_count] = entity;
        ++array.living_count;
        
        return data;
    }

    template<typename T>
    void RemoveComponentData(ComponentArray& array, Entity entity)
    {
        NIT_CHECK_MSG(array.entity_to_index[entity] != INVALID_INDEX, "Removing non-existent component");
        u32 remove_index = array.entity_to_index[entity];
        u32 last_index = array.living_count - 1;
        T* components = static_cast<T*>(array.components);
        
        components[remove_index] = components[last_index];
        array.index_to_entity[remove_index] = array.index_to_entity[last_index];
        array.entity_to_index[entity] = INVALID_INDEX;
        --array.living_count;
    }

    template<typename T>
    T& GetComponentData(ComponentArray& array, Entity entity)
    {
        NIT_CHECK_MSG(array.entity_to_index[entity] != INVALID_INDEX, "Retrieving non-existent component");
        T* components = static_cast<T*>(array.components);
        return components[array.entity_to_index[entity]];
    }

    struct ComponentData
    {
        const char*     name  = "";
        ComponentType  type  = 0;
        ComponentArray* array;
    };

    struct EntityGroup
    {
        EntitySignature signature;
        Set<Entity> entities;
    };

    struct EntityRegistry
    {
        Queue<Entity> available_entities;
        EntitySignature  signatures[MAX_ENTITIES];
        u32 entity_count = 0;
        Map<EntitySignature, EntityGroup> entity_groups;
        ComponentData component_data[MAX_COMPONENTS_TYPES];
        ComponentType next_component_type = 1;
    };
    
    ComponentData* FindComponentDataByName(EntityRegistry& reg, const char* name);
    
    template<typename T>
    void RegisterComponentType(EntityRegistry& reg)
    {
        NIT_CHECK_MSG(!GetComponentData<T>(reg), "Component type already registered!");
        NIT_CHECK_MSG(reg.next_component_type <= MAX_COMPONENTS_TYPES, "Components out of range!");
        ComponentData& component_data = reg.component_data[reg.next_component_type - 1];
        component_data.name  = typeid(T).name();
        component_data.type  = reg.next_component_type;
        component_data.array = new ComponentArray();
        component_data.array->components = new T[MAX_ENTITIES];
        component_data.array->remove_component_func = RemoveComponentData<T>;
        FillRaw(component_data.array->entity_to_index, MAX_ENTITIES, INVALID_INDEX);
        FillRaw(component_data.array->index_to_entity, MAX_ENTITIES, INVALID_INDEX);
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

    template<typename T>
    ComponentData* GetComponentData(EntityRegistry& reg)
    {
        const char* type_name = typeid(T).name();
        return FindComponentDataByName(reg, type_name);
    }
    
    template<typename T>
    ComponentArray& GetComponentArray(EntityRegistry& reg)
    {
        ComponentData* data = GetComponentData<T>(reg);
        NIT_CHECK_MSG(data, "Component type is not registered!");
        return *data->array;
    }

    void EntitySignatureChanged(EntityRegistry& reg, Entity entity, EntitySignature new_entity_signature);
    
    template<typename T>
    T& AddComponent(EntityRegistry& reg, Entity entity)
    {
        NIT_CHECK_MSG(IsEntityValid(reg, entity), "Invalid entity!");
        NIT_CHECK_MSG(reg.signatures[entity].size() <= MAX_COMPONENTS_PER_ENTITY + 1, "Components per entity out of range!");
        T& data = InsertComponentData(GetComponentArray<T>(reg), entity, T());
        EntitySignature& signature = reg.signatures[entity]; 
        signature.set(GetComponentType<T>(reg), true);
        EntitySignatureChanged(reg, entity, signature);
        return data;
    }

    template<typename T>
    void RemoveComponent(EntityRegistry& reg, Entity entity)
    {
        NIT_CHECK_MSG(IsEntityValid(reg, entity), "Invalid entity!");
        NIT_CHECK_MSG(entity < MAX_ENTITIES, "Entity out of range!");
        RemoveComponentData<T>(GetComponentArray<T>(reg), entity, T());
        EntitySignature& signature = reg.signatures[entity]; 
        signature.set(GetComponentType<T>(), false);
        EntitySignatureChanged(reg, entity, signature);
    }

    template<typename T>
    T& GetComponent(EntityRegistry& reg, Entity entity)
    {
        NIT_CHECK_MSG(IsEntityValid(reg, entity), "Invalid entity!");
        return GetComponentData<T>(GetComponentArray<T>(reg), entity);
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
