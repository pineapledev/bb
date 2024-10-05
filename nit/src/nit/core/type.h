#pragma once

namespace Nit
{
    constexpr u32 DEFAULT_MAX_TYPES = 300;

    struct Type
    {
        using FnSetData           = void  (*) (void*, u32, void*);
        using FnGetData           = void* (*) (void*, u32);
        using FnInvokeLoadFree    = void  (*) (void*, void*);
        using FnInvokeSerialize   = void  (*) (void*, void*, YAML::Emitter& emitter);
        using FnInvokeDeserialize = void  (*) (void*, void*, const YAML::Node& node);
        
        String              name                  = "Invalid";
        FnSetData           fn_set_data           = nullptr;
        FnGetData           fn_get_data           = nullptr;
        void*               fn_load               = nullptr;
        void*               fn_free               = nullptr;
        FnInvokeLoadFree    fn_invoke_load_free   = nullptr;
        void*               fn_serialize          = nullptr;
        void*               fn_deserialize        = nullptr;
        FnInvokeSerialize   fn_invoke_serialize   = nullptr;
        FnInvokeDeserialize fn_invoke_deserialize = nullptr;
    };

    template<typename T>
    using FnLoad = void (*) (T*);

    template<typename T>
    void SetInvokeLoadFreeFunction(Type* type)
    {
        NIT_CHECK(type);
        
        type->fn_invoke_load_free = [] (void* fn, void* data) {
            FnLoad<T> casted_fn = static_cast<FnLoad<T>>(fn);
            T* casted_data = static_cast<T*>(data);
            casted_fn(casted_data);
        };
    }
    
    template<typename T>
    void SetLoadFunction(Type* type, FnLoad<T> fn_load)
    {
        NIT_CHECK(type);
        type->fn_load = fn_load;
        if (fn_load && !type->fn_invoke_load_free)
        {
            SetInvokeLoadFreeFunction<T>(type);
        }
    }

    template<typename T>
    using FnFree = FnLoad<T>;
    
    template<typename T>
    void SetFreeFunction(Type* type, FnFree<T> fn_free)
    {
        NIT_CHECK(type);
        type->fn_free = fn_free;
        if (fn_free && !type->fn_invoke_load_free)
        {
            SetInvokeLoadFreeFunction<T>(type);
        }
    }
    
    template<typename T>
    using FnSerialize   = void  (*) (const T*, YAML::Emitter& emitter);

    template<typename T>
    void SetSerializeFunction(Type* type, FnSerialize<T> fn_serialize)
    {
        NIT_CHECK(type);
        
        type->fn_serialize = fn_serialize;

        if (!fn_serialize)
        {
            return;
        }
        
        type->fn_invoke_serialize = [] (void* fn, void* data, YAML::Emitter& emitter) {
            FnSerialize<T> casted_fn = static_cast<FnSerialize<T>>(fn);
            const T* casted_data = static_cast<const T*>(data);
            casted_fn(casted_data, emitter);
        };
    }

    template<typename T>
    using FnDeserialize = void  (*) (T*, const YAML::Node& node);

    template<typename T>
    void SetDeserializeFunction(Type* type, FnDeserialize<T> fn_deserialize)
    {
        NIT_CHECK(type);

        type->fn_deserialize = fn_deserialize;

        if (!fn_deserialize)
        {
            return;
        }
        
        type->fn_invoke_deserialize = [] (void* fn, void* data, const YAML::Node& node) {
            FnDeserialize<T> casted_fn = static_cast<FnDeserialize<T>>(fn);
            T* casted_data = static_cast<T*>(data);
            casted_fn(casted_data, node);
        };
    }
    
    template<typename T>
    struct TypeArgs
    {
        FnLoad<T>        fn_load        = nullptr;
        FnFree<T>        fn_free        = nullptr;
        FnSerialize<T>   fn_serialize   = nullptr;
        FnDeserialize<T> fn_deserialize = nullptr;
    };

    template<typename T>
    void GetTypeName(String& str)
    {
        static const String STRUCT_TEXT = "struct "; 
        static const String CLASS_TEXT  = "class "; 
        str = typeid(T).name();
        Replace(str, STRUCT_TEXT, "");
        Replace(str, CLASS_TEXT , "");
    }
    
    template<typename T>
    void InitType(Type& type, const TypeArgs<T>& args)
    {
        GetTypeName<T>(type.name);
        
        type.fn_set_data = [](void* elements, u32 element_index, void* data) {

            static T default_data;
            T* casted_data = data != nullptr ? static_cast<T*>(data) : nullptr;
            T* casted_elements = static_cast<T*>(elements);
            casted_elements[element_index] = casted_data ? *casted_data : default_data;
        };
        
        type.fn_get_data = [](void* elements, u32 element_index) -> void* {
            T* casted_elements = static_cast<T*>(elements);
            T* data = &casted_elements[element_index];
            return data;
        };
        
        if (auto fn_serialize = args.fn_serialize)
        {
            SetSerializeFunction(&type, fn_serialize);
        }

        if (auto fn_deserialize = args.fn_deserialize)
        {
            SetDeserializeFunction(&type, fn_deserialize);
        }

        if (auto fn_load = args.fn_load)
        {
            SetLoadFunction(&type, fn_load);
        }

        if (auto fn_free = args.fn_free)
        {
            SetFreeFunction(&type, fn_free);
        }
    }

    void  SetRawData(const Type* type, void* array, u32 index, void* data);
    void* GetRawData(const Type* type, void* array, u32 index);
    void  LoadRawData(const Type* type, void* data);
    void  FreeRawData(const Type* type, void* data);
    void  SerializeRawData(const Type* type, void* data, YAML::Emitter& emitter);
    void  DeserializeRawData(const Type* type, void* data, const YAML::Node& node);

    template<typename T>
    void SetData(void* array, u32 index, const T& data)
    {
        T* casted_array = static_cast<T*>(array);
        casted_array[index] = data;
    }

    template<typename T>
    T& GetData(void* array, u32 index)
    {
        T* casted_array = static_cast<T*>(array);
        return casted_array[index];
    }
    
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