#include "type.h"

namespace Nit
{
    TypeRegistry* type_registry = nullptr;

    void SetTypeRegistryInstance(TypeRegistry* type_registry_instance)
    {
        NIT_CHECK(type_registry_instance);
        type_registry = type_registry_instance;
    }

    TypeRegistry* GetTypeRegistryInstance()
    {
        NIT_CHECK(type_registry);
        return type_registry;
    }
    
    void SetRawData(const Type* type, void* array, u32 index, void* data)
    {
        NIT_CHECK(type && type->fn_set_data && array);
        type->fn_set_data(array, index, data);
    }

    void* GetRawData(const Type* type, void* array, u32 index)
    {
        NIT_CHECK(type && type->fn_get_data && array);
        return type->fn_get_data(array, index);
    }

    void LoadRawData(const Type* type, void* data)
    {
        NIT_CHECK(type);
        if (type->fn_load)
        {
            NIT_CHECK(type->fn_invoke_load_free && data);
            type->fn_invoke_load_free(type->fn_load, data);    
        }
    }

    void FreeRawData(const Type* type, void* data)
    {
        NIT_CHECK(type);
        if (type->fn_free)
        {
            NIT_CHECK(type->fn_invoke_load_free && data);
            type->fn_invoke_load_free(type->fn_free, data);    
        }
    }

    void SerializeRawData(const Type* type, void* data, YAML::Emitter& emitter)
    {
        NIT_CHECK(type);
        if (type->fn_serialize)
        {
            NIT_CHECK(type->fn_invoke_serialize && data);
            type->fn_invoke_serialize(type->fn_serialize, data, emitter);
        }
    }

    void DeserializeRawData(const Type* type, void* data, const YAML::Node& node)
    {
        NIT_CHECK(type);
        if (type->fn_deserialize)
        {
            NIT_CHECK(type->fn_invoke_deserialize && data);
            type->fn_invoke_deserialize(type->fn_deserialize, data, node);
        }
    }

    void InitTypeRegistry(u32 max_types)
    {
        NIT_CHECK(type_registry && !type_registry->types);
        type_registry->types = new Type[max_types];
        type_registry->max   = max_types;
        type_registry->count = 0;
    }

    bool IsTypeRegistered(const String& type_name)
    {
        NIT_CHECK(type_registry);
        return type_registry->name_to_index.count(type_name) != 0;
    }

    Type* GetType(const String& type_name)
    {
        NIT_CHECK(type_registry && type_registry->types);
        return &type_registry->types[type_registry->name_to_index[type_name]];
    }
}