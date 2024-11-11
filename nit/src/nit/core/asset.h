#pragma once

namespace nit
{
    struct AssetInfo
    {
        Type*  type;
        String name;
        String path;
        UUID   id              = {};
        u32    version         = 0;
        bool   loaded          = false;
        u32    reference_count = 0;
        u32    data_id         = SparseSet::INVALID;
    };
    
    template<typename T>
    struct AssetTypeArgs
    {
        FnLoad<T>        fn_load        = nullptr;
        FnFree<T>        fn_free        = nullptr;
        FnSerialize<T>   fn_serialize   = nullptr;
        FnDeserialize<T> fn_deserialize = nullptr;
#ifdef NIT_EDITOR_ENABLED
        FnDrawEditor<T>  fn_draw_editor = nullptr;
#endif
        u32              max_elements   = 100;
    };

    struct AssetHandle
    {
        String name;
        Type*  type    = nullptr;
        UUID   id;
        u32    data_id = SparseSet::INVALID;
    };
    
    struct AssetCreatedArgs
    {
        AssetHandle asset_handle;
    };
    
    struct AssetDestroyedArgs
    {
        AssetHandle asset_handle;
    };
    
    using AssetCreatedEvent      = Event<const AssetCreatedArgs&>;
    using AssetRemovedEvent      = Event<const AssetDestroyedArgs&>;
    using AssetCreatedListener   = Listener<const AssetCreatedArgs&>;
    using AssetDestroyedListener = Listener<const AssetDestroyedArgs&>;

    struct AssetPool
    {
        Pool         data_pool;
        u32          version       = 0;
        AssetInfo*   asset_infos   = nullptr;
    };
    
    struct AssetRegistry
    {
        Array<AssetPool>     asset_pools;
        String               extension = ".nit";
        AssetCreatedEvent    asset_created_event;
        AssetRemovedEvent    asset_destroyed_event;
        Map<UUID, u32>       id_to_data_id;
    };

    AssetHandle create_asset_handle(AssetInfo* asset_info);
    void retarget_asset_handle(AssetHandle& asset_handle);

    Path get_assets_directory();
    
    void set_asset_registry_instance(AssetRegistry* asset_registry_instance);
    
    AssetRegistry* get_asset_registry_instance();
    
    template<typename T>
    void register_asset_type(const AssetTypeArgs<T>& args, u32 version = 0);

    AssetPool* get_asset_pool(Type* type);
    
    AssetPool* get_asset_pool_safe(const AssetHandle& asset);
    
    AssetPool* get_asset_pool_safe(Type* type);
    
    template<typename T>
    AssetPool* get_asset_pool();

    bool is_asset_type_registered(Type* type);
    
    template<typename T>
    bool is_asset_type_registered();
    
    void build_asset_path(const String& name, String& path);

    void push_asset_info(AssetInfo& asset_info, u32 index, bool build_path);
    void erase_asset_info(AssetInfo& asset_info, SparseSetDeletion deletion);
    
    AssetInfo* get_asset_info(AssetHandle& asset);
    
    AssetHandle deserialize_asset_from_string(const String& asset_str);
    
    AssetHandle deserialize_asset_from_file(const String& file_path);
    
    void serialize_asset_to_string(AssetHandle& asset, String& result);
    
    void serialize_asset_to_file(AssetHandle& asset);
    
    void init_asset_registry();
    
    u32 get_last_asset_version(Type* type);
    
    u32 get_last_asset_version(const String& type_name);

    template<typename T>
    u32 get_last_asset_version();
    
    void find_assets_by_name(const String& name, Array<AssetHandle>& assets);
    
    AssetHandle find_asset_by_name(const String& name);

    void get_assets_of_type(Type* type, Array<AssetHandle>& assets);
    
    template<typename T>
    T* get_asset_data(AssetHandle& asset);

    bool is_asset_valid(AssetHandle& asset);

    bool is_asset_loaded(AssetHandle& asset);

    AssetHandle create_asset(Type* type, const String& name, const String& path, void* data = nullptr);
    
    template<typename T>
    AssetHandle create_asset(const String& name, const String& path = "", const T& data = {});
    
    void load_asset(AssetHandle& asset, bool force_reload = false);
    
    void free_asset(AssetHandle& asset);

    void retain_asset(AssetHandle& asset);
    
    void release_asset(AssetHandle& asset, bool force_free = false);
    
    void destroy_asset(AssetHandle& asset);
}

#include "asset.inl"