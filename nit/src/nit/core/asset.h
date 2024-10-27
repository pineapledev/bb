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

    struct AssetHandle
    {
        String name;
        Type*  type;
        ID     id;
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
    AssetPool* GetAssetPoolSafe(const AssetHandle& asset);
    AssetPool* GetAssetPoolSafe(Type* type);
    
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
    
    AssetInfo* GetAssetInfo(const AssetHandle& asset);
    
    AssetHandle DeserializeAssetFromString(const String& asset_str);
    
    AssetHandle DeserializeAssetFromFile(const String& file_path);
    
    void SerializeAssetToString(const AssetHandle& asset, String& result);
    
    void SerializeAssetToFile(const AssetHandle& asset);
    
    void InitAssetRegistry();
    
    u32 GetLastAssetVersion(Type* type);
    
    u32 GetLastAssetVersion(const String& type_name);

    template<typename T>
    u32 GetLastAssetVersion()
    {
        return GetLastAssetVersion(GetType<T>());
    }
    
    void FindAssetsByName(const String& name, Array<AssetHandle>& assets);
    
    AssetHandle FindAssetByName(const String& name);

    void GetAssetsOfType(Type* type, Array<AssetHandle>& assets);
    
    template<typename T>
    T* GetAssetData(const AssetHandle& asset)
    {
        AssetPool* pool = GetAssetPoolSafe(GetType<T>());
        return GetData<T>(&pool->data_pool, asset.id);
    }

    template<typename T>
    T* GetAssetDataPtr(const AssetHandle& asset)
    {
        AssetPool* pool = GetAssetPoolSafe(GetType<T>());
        return GetDataPtr<T>(&pool->data_pool, asset.id);
    }

    bool IsAssetValid(const AssetHandle& asset);

    bool IsAssetLoaded(const AssetHandle& asset);
    
    template<typename T>
    AssetHandle CreateAsset(const String& name, const String& path = "", const T& data = {})
    {
        AssetPool* pool = GetAssetPoolSafe(GetType<T>());
        PoolMap* data_pool = &pool->data_pool;
        Type* type = data_pool->type;
        ID id; InsertData(data_pool, id, data);
        AssetInfo info{type->name, name, path, id, GetLastAssetVersion<T>() };
        PushAssetInfo(info, IndexOf(data_pool, id), true);
        AssetHandle asset_handle{ name, type, id };
        Broadcast<const AssetCreatedArgs&>(GetAssetRegistryInstance()->asset_created_event, {asset_handle});
        return asset_handle;
    }
    
    void LoadAsset(AssetHandle& asset, bool force_reload = false);
    
    void FreeAsset(AssetHandle& asset);

    void RetainAsset(AssetHandle& asset);
    
    void ReleaseAsset(AssetHandle& asset, bool force_free = false);
    
    void DestroyAsset(AssetHandle& asset);
}

template<>
struct YAML::convert<Nit::AssetHandle>
{
    static Node encode(const Nit::AssetHandle& h)
    {
        Node node;
        node.push_back(h.name);
        node.push_back(h.type->name);
        node.push_back(h.id);
        node.SetStyle(EmitterStyle::Flow);
        return node;
    }
    
    static bool decode(const Node& node, Nit::AssetHandle& h)
    {
        if (!node.IsSequence() || node.size() != 3)
            return false;
        
        h.name = node[0].as<Nit::String>();
        h.type = Nit::GetType(node[1].as<Nit::String>());
        h.id   = node[2].as<Nit::ID>();
        return true;
    }
};

inline YAML::Emitter& operator<<(YAML::Emitter& out, const Nit::AssetHandle& h)
{
    out << YAML::Flow;
    out << YAML::BeginSeq << h.name << h.type->name << h.id << YAML::EndSeq;
    return out;
}