#pragma once

namespace Nit
{
    constexpr u32 DEFAULT_MAX_TYPES = 300;

    struct Type
    {
        using FnSetData           = void  (*) (void*, u32, void*);
        using FnGetData           = void* (*) (void*, u32);
        using FnInvokeLoad        = Function<void(void*)>;
        using FnInvokeFree        = Function<void(void*)>;
        using FnInvokeSerialize   = Function<void(void*, YAML::Emitter& emitter)>;
        using FnInvokeDeserialize = Function<void(void*, const YAML::Node& node)>;

        String              name;
        u64                 hash                  = 0;
        FnSetData           fn_set_data           = nullptr;
        FnGetData           fn_get_data           = nullptr;
        FnInvokeLoad        fn_invoke_load        = nullptr;
        FnInvokeFree        fn_invoke_free        = nullptr;
        FnInvokeSerialize   fn_invoke_serialize   = nullptr;
        FnInvokeDeserialize fn_invoke_deserialize = nullptr;
    };

    template<typename T>
    using FnLoad = void (*) (T*);
    
    template<typename T>
    void SetInvokeLoadFunction(Type* type, FnLoad<T> fn_load)
    {
        NIT_CHECK(type);
        if (fn_load && !type->fn_invoke_load)
        {
            type->fn_invoke_load = [fn_load] (void* data) {
                T* casted_data = static_cast<T*>(data);
                fn_load(casted_data);
            };
        }
    }

    template<typename T>
    using FnFree = FnLoad<T>;

    template<typename T>
    void SetInvokeFreeFunction(Type* type, FnFree<T> fn_free)
    {
        NIT_CHECK(type);
        if (fn_free && !type->fn_invoke_free)
        {
            type->fn_invoke_free = [fn_free] (void* data) {
                T* casted_data = static_cast<T*>(data);
                fn_free(casted_data);
            };
        }
    }
    
    template<typename T>
    using FnSerialize = void (*) (const T*, YAML::Emitter& emitter);

    template<typename T>
    void SetSerializeFunction(Type* type, FnSerialize<T> fn_serialize)
    {
        NIT_CHECK(type);
        
        if (fn_serialize)
        {
            type->fn_invoke_serialize = [fn_serialize] (void* data, YAML::Emitter& emitter) {
                const T* casted_data = static_cast<const T*>(data);
                fn_serialize(casted_data, emitter);
            };
        }
    }

    template<typename T>
    using FnDeserialize = void (*) (T*, const YAML::Node& node);

    template<typename T>
    void SetDeserializeFunction(Type* type, FnDeserialize<T> fn_deserialize)
    {
        NIT_CHECK(type);
        
        if (fn_deserialize)
        {
            type->fn_invoke_deserialize = [fn_deserialize] (void* data, const YAML::Node& node) {
                T* casted_data = static_cast<T*>(data);
                fn_deserialize(casted_data, node);
            };
        }
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
    u64 GetTypeHash()
    {
        return typeid(T).hash_code();
    }
    
    template<typename T>
    void InitType(Type& type, const TypeArgs<T>& args)
    {
        type.hash = GetTypeHash<T>();
        
        static const String STRUCT_TEXT = "struct "; 
        static const String CLASS_TEXT  = "class "; 
        static const String NAMESPACE_TEXT  = "Nit::"; 
        type.name = typeid(T).name();
        Replace(type.name, STRUCT_TEXT, "");
        Replace(type.name, CLASS_TEXT , "");
        Replace(type.name, NAMESPACE_TEXT , "");
        
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
            SetInvokeLoadFunction(&type, fn_load);
        }

        if (auto fn_free = args.fn_free)
        {
            SetInvokeFreeFunction(&type, fn_free);
        }
    }

    void  SetData(const Type* type, void* array, u32 index, void* data);
    void* GetData(const Type* type, void* array, u32 index);
    void  Load(const Type* type, void* data);
    void  Free(const Type* type, void* data);
    void  Serialize(const Type* type, void* data, YAML::Emitter& emitter);
    void  Deserialize(const Type* type, void* data, const YAML::Node& node);

    template<typename T>
    T& SetData(void* array, u32 index, const T& data)
    {
        T* casted_array = static_cast<T*>(array);
        T& saved_data = casted_array[index];
        saved_data = data;
        return saved_data;
    }

    template<typename T>
    T* GetDataPtr(void* array, u32 index)
    {
        T* casted_array = static_cast<T*>(array);
        return &casted_array[index];
    }
    
    template<typename T>
    T& GetData(void* array, u32 index)
    {
        T* casted_array = static_cast<T*>(array);
        return casted_array[index];
    }

    struct EnumType
    {
        String name;
        u64    hash  = 0;
        Map<String, u8> name_to_index;
        Map<u8, String> index_to_name;
    };

    template <typename T>
    void InitEnumType(EnumType& enum_type)
    {
        enum_type.hash = typeid(T).hash_code();
        static const String ENUM_TEXT  = "enum class "; 
        static const String ENUM_CLASS_TEXT  = "enum "; 
        enum_type.name = typeid(T).name();
        Replace(enum_type.name, ENUM_TEXT , "");
        Replace(enum_type.name, ENUM_CLASS_TEXT , "");
    }

    template <typename T>
    void RegisterEnumValue(EnumType* enum_type, const String& value_name, T value)
    {
        NIT_CHECK(enum_type);
        u8 index = (u8) value;
        enum_type->name_to_index.insert({ value_name, index });
        enum_type->index_to_name.insert({ index, value_name });
    }

    template <typename T>
    T GetEnumValueFromString(EnumType* enum_type, const String& value_name)
    {
        NIT_CHECK(enum_type && enum_type->name_to_index.count(value_name) != 0);
        return (T) enum_type->name_to_index.at(value_name);
    }

    template <typename T>
    String GetStringFromEnumValue(EnumType* enum_type, T value)
    {
        u8 index = (u8) value;
        NIT_CHECK(enum_type && enum_type->index_to_name.count(index) != 0);
        return enum_type->index_to_name.at(index);
    }
    
    struct TypeRegistry
    {
        Type*             types              = nullptr;
        u32               count              = 0;
        u32               max                = 0;
        Map<u64, u32>     hash_to_index      = {};
        Map<String, u64>  name_to_index      = {};
        EnumType*         enum_types         = nullptr;
        u32               enum_count         = 0;
        u32               max_enum_types     = 0;
        Map<u64, u32>     hash_to_enum_index = {};    
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
        NIT_CHECK(type_registry->hash_to_index.count(type.hash) == 0);
        type_registry->hash_to_index[type.hash] = type_registry->count;
        NIT_CHECK(type_registry->name_to_index.count(type.name) == 0);
        type_registry->name_to_index[type.name] = type_registry->count;
        ++type_registry->count;
    }

    template <typename T>
    void RegisterEnumType()
    {
        TypeRegistry* type_registry = GetTypeRegistryInstance();
        if (type_registry->enum_count >= type_registry->max_enum_types)
        {
            NIT_CHECK_MSG(false, "Max enum type count reached!");
            return;
        }
        
        EnumType& enum_type = type_registry->enum_types[type_registry->enum_count];
        InitEnumType<T>(enum_type);
        NIT_CHECK(type_registry->hash_to_index.count(enum_type.hash) == 0);
        type_registry->hash_to_enum_index[enum_type.hash] = type_registry->enum_count;
        ++type_registry->enum_count;
    }

    bool IsEnumTypeRegistered(u64 type_hash);

    template <typename T>
    bool IsEnumTypeRegistered()
    {
        return IsEnumTypeRegistered(GetTypeHash<T>());
    }

    EnumType* GetEnumType(u64 type_hash);

    template <typename T>
    EnumType* GetEnumType()
    {
        return GetEnumType(GetTypeHash<T>());
    }

    template <typename EType, typename T>
    void RegisterEnumValue(const String& value_name, T value)
    {
        EnumType* enum_type = GetEnumType<EType>();
        u8 index = (u8) value;
        enum_type->name_to_index.insert({ value_name, index });
        enum_type->index_to_name.insert({ index, value_name });
    }
    
    template <typename T>
    T GetEnumValueFromString(const String& value_name)
    {
        return GetEnumValueFromString<T>(GetEnumType<T>(), value_name);
    }
    
    template <typename EType, typename T>
    String GetStringFromEnumValue(T value)
    {
        return GetStringFromEnumValue(GetEnumType<EType>(), value);
    }
    
    bool IsTypeRegistered(u64 type_hash);
    bool IsTypeRegistered(const String& name);
    
    template <typename T>
    bool IsTypeRegistered()
    {
        return IsTypeRegistered(GetTypeHash<T>());
    }
    
    Type* GetType(u64 type_hash);
    
    Type* GetType(const String& name);
    
    template <typename T>
    Type* GetType()
    {
        return GetType(GetTypeHash<T>());
    }
}