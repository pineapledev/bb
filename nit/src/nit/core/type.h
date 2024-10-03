#pragma once

namespace Nit
{
    constexpr u32 DEFAULT_MAX_TYPES = 300;

    //TODO: Add load / free callbacks
    struct Type
    {
        using FnSetData           = void  (*) (void*, u32, void*);
        using FnGetData           = void* (*) (void*, u32);
        using FnInvokeSerialize   = void  (*) (void*, void*, YAML::Emitter& emitter);
        using FnInvokeDeserialize = void  (*) (void*, void*, const YAML::Node& node);
        
        String              name                  = "Invalid";
        FnSetData           fn_set_data           = nullptr;
        FnGetData           fn_get_data           = nullptr;
        void*               fn_serialize          = nullptr;
        void*               fn_deserialize        = nullptr;
        FnInvokeSerialize   fn_invoke_serialize   = nullptr;
        FnInvokeDeserialize fn_invoke_deserialize = nullptr;
    };

    template<typename T>
    using FnSerialize   = void  (*) (const T*, YAML::Emitter& emitter);

    template<typename T>
    void SetTypeSerializeFunction(Type* type, FnSerialize<T> fn_serialize)
    {
        NIT_CHECK(fn_serialize);
        
        type->fn_serialize = fn_serialize;
            
        type->fn_invoke_serialize = [] (void* fn, void* data, YAML::Emitter& emitter) {
            FnSerialize<T> casted_fn = static_cast<FnSerialize<T>>(fn);
            const T* casted_data = static_cast<const T*>(data);
            casted_fn(casted_data, emitter);
        };
    }

    template<typename T>
    using FnDeserialize = void  (*) (T*, const YAML::Node& node);

    template<typename T>
    void SetTypeDeserializeFunction(Type* type, FnDeserialize<T> fn_deserialize)
    {
        NIT_CHECK(fn_deserialize);

        type->fn_deserialize = fn_deserialize;
            
        type->fn_invoke_deserialize = [] (void* fn, void* data, const YAML::Node& node) {
            FnDeserialize<T> casted_fn = static_cast<FnDeserialize<T>>(fn);
            T* casted_data = static_cast<T*>(data);
            casted_fn(casted_data, node);
        };
    }
    
    template<typename T>
    struct TypeArgs
    {
        FnSerialize<T>   fn_serialize   = nullptr;
        FnDeserialize<T> fn_deserialize = nullptr;
    };

    template<typename T>
    void GetTypeName(String& str)
    {
        static const String STRUCT_TEXT = "struct"; 
        static const String CLASS_TEXT  = "class"; 
        str = typeid(T).name();
        Replace(str, STRUCT_TEXT, "");
        Replace(str, CLASS_TEXT , "");
    }
    
    template<typename T>
    void InitType(Type& type, const TypeArgs<T>& args)
    {
        GetTypeName<T>(type.name);
        
        type.fn_set_data = [](void* elements, u32 element_index, void* data) {
            T* casted_data     = static_cast<T*>(data);
            T* casted_elements = static_cast<T*>(elements);
            casted_elements[element_index] = *casted_data;
        };
        
        type.fn_get_data = [](void* elements, u32 element_index) -> void* {
            T* casted_elements = static_cast<T*>(elements);
            T* data = &casted_elements[element_index];
            return data;
        };
        
        if (FnSerialize<T> fn_serialize = args.fn_serialize)
        {
            SetTypeSerializeFunction(&type, fn_serialize);
        }

        if (FnDeserialize<T> fn_deserialize = args.fn_deserialize)
        {
            SetTypeDeserializeFunction(&type, fn_deserialize);
        }
    }

    void  SetRawData(const Type* type, void* array, u32 index, void* data);
    void* GetRawData(const Type* type, void* array, u32 index);
    void  SerializeRawData(const Type* type, void* data, YAML::Emitter& emitter);
    void  DeserializeRawData(const Type* type, void* data, const YAML::Node& node);
    
    struct TypeRegistry
    {
        Type*             types         = nullptr;
        u32               count         = 0;
        u32               max           = 0;
        Map<String, u32>  name_to_index = {};
    };

    void SetTypeRegistryInstance(TypeRegistry* type_registry_instance);
    TypeRegistry* GetTypeRegistryInstance();
    
    void InitTypeRegistry(u32 max_types = DEFAULT_MAX_TYPES);
    
    template <typename T>
    void RegisterType(const TypeArgs<T>& args = {})
    {
        TypeRegistry* type_registry = GetTypeRegistryInstance();
        
        if (type_registry->count >= type_registry->max)
        {
            NIT_CHECK_MSG(false, "Max type count reached!");
            return;
        }
        Type& type = type_registry->types[type_registry->count]; 
        InitType(type, args);
        NIT_CHECK(type_registry->name_to_index.count(type.name) == 0);
        type_registry->name_to_index[type.name] = type_registry->count;
        ++type_registry->count;
    }

    bool IsTypeRegistered(const String& type_name);
    
    template <typename T>
    bool IsTypeRegistered()
    {
        String type_name;
        GetTypeName<T>(type_name);
        return IsTypeRegistered(type_name);
    }
    
    Type* GetType(const String& type_name);
    
    template <typename T>
    Type* GetType()
    {
        String type_name;
        GetTypeName<T>(type_name);
        return GetType(type_name);
    }
}