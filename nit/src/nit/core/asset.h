#pragma once

namespace Nit
{
    struct AssetInfo
    {
        String type_name;
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
        u32              max_elements   = PoolMap::DEFAULT_MAX;
    };

    struct AssetCreatedArgs
    {
        ID    id    = 0;
        Type* type  = nullptr;
    };
    
    struct AssetDestroyedArgs
    {
        ID    id    = 0;
        Type* type  = nullptr;
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
        Array<AssetPool>    asset_pools;
        String              extension = ".nit";
        AssetCreatedEvent   asset_created_event;
        AssetRemovedEvent   asset_destroyed_event;
    };
    
    void SetAssetRegistryInstance(AssetRegistry* asset_registry_instance);
    
    AssetRegistry* GetAssetRegistryInstance();
    
    template<typename T>
    void RegisterAssetType(const AssetTypeArgs<T>& args, u32 version = 0)
    {
        AssetRegistry* asset_registry = GetAssetRegistryInstance();
        AssetPool* asset_pool = &asset_registry->asset_pools.emplace_back();
        
        if (!IsTypeRegistered<T>())
        {
            RegisterType<T>();
        }
        
        Type* type = GetType<T>();
        SetInvokeLoadFunction(type, args.fn_load);
        SetInvokeFreeFunction(type, args.fn_free);
        SetSerializeFunction(type, args.fn_serialize);
        SetDeserializeFunction(type, args.fn_deserialize);
        
        Load<T>(&asset_pool->data_pool, args.max_elements);
        asset_pool->asset_infos = new AssetInfo[args.max_elements];
    }

    AssetPool* GetAssetPool(Type* type);
    
    template<typename T>
    AssetPool* GetAssetPool()
    {
        return GetAssetPool(GetType<T>());
    }

    bool IsAssetTypeRegistered(Type* type);
    
    template<typename T>
    bool IsAssetTypeRegistered()
    {
        return IsAssetTypeRegistered(GetType<T>());
    }
    
    void BuildAssetPath(const String& name, String& path);

    void PushAssetInfo(AssetInfo& asset_info, u32 index, bool build_path);
    void EraseAssetInfo(AssetInfo& asset_info, SparseSetDeletion deletion);

    AssetInfo* GetAssetInfo(Type* type, ID id);
    
    ID DeserializeAssetFromString(const String& asset_str);
    
    ID DeserializeAssetFromFile(const String& file_path);
    
    void SerializeAssetToString(Type* type, ID id, String& result);
    
    void SerializeAssetToFile(Type* type, ID id);
    
    void InitAssetRegistry();
    
    u32 GetLastAssetVersion(Type* type);
    
    u32 GetLastAssetVersion(const String& type_name);

    template<typename T>
    u32 GetLastAssetVersion()
    {
        return GetLastAssetVersion(GetType<T>());
    }

    void FindAssetsByName(const String& name, Array<ID>& asset_ids);
    
    ID FindAssetByName(const String& name);
    
    template<typename T>
    T& GetAssetData(ID id)
    {
        PoolMap& pool = GetAssetPool<T>();
        return GetData<T>(&pool, id);
    }

    template<typename T>
    T* GetAssetDataPtr(ID id)
    {
        AssetPool* pool = GetAssetPool<T>();
        return GetDataPtr<T>(&pool->data_pool, id);
    }

    bool IsAssetValid(Type* type, ID id);

    bool IsAssetLoaded(Type* type, ID id);
    
    template<typename T>
    ID CreateAsset(const String& name, const String& path = "", const T& data = {})
    {
        AssetPool* pool = GetAssetPool<T>();
        ID id; InsertData(&pool->data_pool, id, data);
        AssetInfo info { pool->data_pool.type->name, name, path, id, GetLastAssetVersion<T>(), };
        PushAssetInfo(info, IndexOf(&pool->data_pool, id), true);
        AssetCreatedArgs args;
        args.id   = id;
        args.type = pool->data_pool.type;
        Broadcast<const AssetCreatedArgs&>(GetAssetRegistryInstance()->asset_created_event, args);
        return id;
    }
    
    void LoadAsset(Type* type, ID id, bool force_reload = false);
    
    void FreeAsset(Type* type, ID id);

    void RetainAsset(Type* type, ID id);
    
    void ReleaseAsset(Type* type, ID id, bool force_free = false);
    
    void DestroyAsset(Type* type, ID id);
}
