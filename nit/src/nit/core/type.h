#pragma once

namespace nit
{
    constexpr u32 DEFAULT_MAX_TYPES = 300;

    struct Type
    {
        using FnSetData           = void  (*) (void*, u32, void*);
        using FnGetData           = void* (*) (void*, u32);
        using FnResizeData        = void* (*) (void*, u32, u32);
        using FnInvokeLoad        = Function<void(void*)>;
        using FnInvokeFree        = Function<void(void*)>;
        using FnInvokeSerialize   = Function<void(void*, YAML::Emitter& emitter)>;
        using FnInvokeDeserialize = Function<void(void*, const YAML::Node& node)>;

#ifdef NIT_EDITOR_ENABLED
        using FnInvokeDrawEditor  = Function<void(void*)>;
#endif
        
        String              name;
        u64                 hash                  = 0;
        FnSetData           fn_set_data           = nullptr;
        FnGetData           fn_get_data           = nullptr;
        FnResizeData        fn_resize_data        = nullptr;
        FnInvokeLoad        fn_invoke_load        = nullptr;
        FnInvokeFree        fn_invoke_free        = nullptr;
        FnInvokeSerialize   fn_invoke_serialize   = nullptr;
        FnInvokeDeserialize fn_invoke_deserialize = nullptr;

#ifdef NIT_EDITOR_ENABLED
        FnInvokeDrawEditor  fn_invoke_draw_editor = nullptr;
#endif
    };

    struct EnumType
    {
        String name;
        u64    hash  = 0;
        Map<String, u8> name_to_index;
        Map<u8, String> index_to_name;
    };

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

    template<typename T>
    using FnLoad = void (*) (T*);

    template<typename T>
    using FnFree = FnLoad<T>;

    template<typename T>
    using FnSerialize = void (*) (const T*, YAML::Emitter& emitter);

    template<typename T>
    using FnDeserialize = void (*) (T*, const YAML::Node& node);

#ifdef NIT_EDITOR_ENABLED
    template<typename T>
    using FnDrawEditor = void (*) (T*);
#endif
    
    template<typename T>
    struct TypeArgs
    {
        FnLoad<T>        fn_load        = nullptr;
        FnFree<T>        fn_free        = nullptr;
        FnSerialize<T>   fn_serialize   = nullptr;
        FnDeserialize<T> fn_deserialize = nullptr;
#ifdef NIT_EDITOR_ENABLED
        FnDrawEditor<T>  fn_draw_editor = nullptr;
#endif
    };
    
    template<typename T>
    void set_invoke_load_function(Type* type, FnLoad<T> fn_load);
    template<typename T>
    void set_invoke_free_function(Type* type, FnFree<T> fn_free);

    template<typename T>
    void set_serialize_function(Type* type, FnSerialize<T> fn_serialize);

    template<typename T>
    void set_deserialize_function(Type* type, FnDeserialize<T> fn_deserialize);

#ifdef NIT_EDITOR_ENABLED
    template<typename T>
    void set_draw_editor_function(Type* type, FnDrawEditor<T> fn_draw_editor);
#endif

    template<typename T>
    u64 get_type_hash();
    
    template<typename T>
    void init_type(Type& type, const TypeArgs<T>& args);

    void  set_array_raw_data(const Type* type, void* array, u32 index, void* data);
    void* get_array_raw_data(const Type* type, void* array, u32 index);
    void  resize_array(const Type* type, void* array, u32 max, u32 new_max);
    void  load(const Type* type, void* data);
    void  type_release(const Type* type, void* data);
    void  serialize(const Type* type, void* data, YAML::Emitter& emitter);
    void  deserialize(const Type* type, void* data, const YAML::Node& node);
    
#ifdef NIT_EDITOR_ENABLED
    void  type_draw_editor(const Type* type, void* data);
#endif

    template<typename T>
    T* SetArrayData(void* array, u32 index, const T& data);

    template<typename T>
    T* GetArrayData(void* array, u32 index);

    template <typename T>
    void InitEnumType(EnumType& enum_type);

    template <typename T>
    void RegisterEnumValue(EnumType* enum_type, const String& value_name, T value);

    template <typename T>
    T GetEnumValueFromString(EnumType* enum_type, const String& value_name);

    template <typename T>
    String GetStringFromEnumValue(EnumType* enum_type, T value);
    
    void          type_registry_set_instance(TypeRegistry* type_registry_instance);
    TypeRegistry* type_registry_get_instance();
    bool          type_registry_has_instance();
    void          type_registry_init(u32 max_types = DEFAULT_MAX_TYPES);
    
    template <typename T>
    void type_register(const TypeArgs<T>& args = {});

    template <typename T>
    void RegisterEnumType();

    bool IsEnumTypeRegistered(u64 type_hash);

    template <typename T>
    bool IsEnumTypeRegistered();

    EnumType* GetEnumType(u64 type_hash);

    template <typename T>
    EnumType* GetEnumType();

    template <typename EType, typename T>
    void RegisterEnumValue(const String& value_name, T value);
    
    template <typename T>
    T GetEnumValueFromString(const String& value_name);
    
    template <typename EType, typename T>
    String GetStringFromEnumValue(T value);
    
    bool type_exists(u64 type_hash);
    bool type_exists(const String& name);
    
    template <typename T>
    bool type_exists();
    
    Type* GetType(u64 type_hash);
    
    Type* GetType(const String& name);
    
    template <typename T>
    Type* GetType();
}

#include "type.inl"