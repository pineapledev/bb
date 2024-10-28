#pragma once

namespace Nit
{
    struct AssetInfo
    {
        Type*  type;
        String name;
        String path;
        ID     id              = 0;
        u32    version         = 0;
        bool   loaded          = false;
        u32    reference_count = 0;
    };
    
    template<typename T>
    struct AssetTypeArgs
    {
        FnLoad<T>        fn_load        = nullptr;
        FnFree<T>        fn_free        = nullptr;
        FnSerialize<T>   fn_serialize   = nullptr;
        FnDeserialize<T> fn_deserialize = nullptr;
        FnDrawEditor<T>  fn_draw_editor = nullptr;
        u32              max_elements   = PoolMap::DEFAULT_MAX;
    };

    struct AssetHandle
    {
        String name;
        Type*  type = nullptr;
        ID     id   = 0;
    };
    
    struct AssetCreatedArgs
    {
        AssetHandle asset_handle;
    };
    
    struct AssetDestroyedArgs
    {
        AssetHandle asset_handle;
    };

    using AssetCreatedEvent = Event<const AssetCreatedArgs&>;
    using AssetRemovedEvent = Event<const AssetDestroyedArgs&>;

    struct AssetPool
    {
        PoolMap    data_pool;
        u32        version     = 0;
        AssetInfo* asset_infos = nullptr;
    };
    
    struct AssetRegistry
    {
        Array<AssetPool>     asset_pools;
        String               extension = ".nit";
        AssetCreatedEvent    asset_created_event;
        AssetRemovedEvent    asset_destroyed_event;
    };
    
    void SetAssetRegistryInstance(AssetRegistry* asset_registry_instance);
    
    AssetRegistry* GetAssetRegistryInstance();
    
    template<typename T>
    void RegisterAssetType(const AssetTypeArgs<T>& args, u32 version = 0);

    AssetPool* GetAssetPool(Type* type);
    
    AssetPool* GetAssetPoolSafe(const AssetHandle& asset);
    
    AssetPool* GetAssetPoolSafe(Type* type);
    
    template<typename T>
    AssetPool* GetAssetPool();

    bool IsAssetTypeRegistered(Type* type);
    
    template<typename T>
    bool IsAssetTypeRegistered();
    
    void BuildAssetPath(const String& name, String& path);

    void PushAssetInfo(AssetInfo& asset_info, u32 index, bool build_path);
    void EraseAssetInfo(AssetInfo& asset_info, SparseSetDeletion deletion);
    
    AssetInfo* GetAssetInfo(const AssetHandle& asset);
    
    AssetHandle DeserializeAssetFromString(const String& asset_str);
    
    AssetHandle DeserializeAssetFromFile(const String& file_path);
    
    void SerializeAssetToString(const AssetHandle& asset, String& result);
    
    void SerializeAssetToFile(const AssetHandle& asset);
    
    void InitAssetRegistry();
    
    u32 GetLastAssetVersion(Type* type);
    
    u32 GetLastAssetVersion(const String& type_name);

    template<typename T>
    u32 GetLastAssetVersion();
    
    void FindAssetsByName(const String& name, Array<AssetHandle>& assets);
    
    AssetHandle FindAssetByName(const String& name);

    void GetAssetsOfType(Type* type, Array<AssetHandle>& assets);
    
    template<typename T>
    T* GetAssetData(const AssetHandle& asset);

    template<typename T>
    T* GetAssetDataPtr(const AssetHandle& asset);

    bool IsAssetValid(const AssetHandle& asset);

    bool IsAssetLoaded(const AssetHandle& asset);
    
    template<typename T>
    AssetHandle CreateAsset(const String& name, const String& path = "", const T& data = {});
    
    void LoadAsset(AssetHandle& asset, bool force_reload = false);
    
    void FreeAsset(AssetHandle& asset);

    void RetainAsset(AssetHandle& asset);
    
    void ReleaseAsset(AssetHandle& asset, bool force_free = false);
    
    void DestroyAsset(AssetHandle& asset);
}

#include "asset.inl"