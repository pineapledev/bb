#pragma once

namespace Nit
{
    struct AssetInfo
    {
        String type_name;
        String name;
        String path;
        ID     id              = 0;
        u32    current_version = 0;
        u32    last_version    = 0;
    };
    
    template<typename T>
    struct AssetTypeArgs
    {
        FnLoad<T>        fn_load        = nullptr;
        FnFree<T>        fn_free        = nullptr;
        FnSerialize<T>   fn_serialize   = nullptr;
        FnDeserialize<T> fn_deserialize = nullptr;
        u32              max_elements   = DEFAULT_POOL_ELEMENT_COUNT;
    };

    //TODO: Finds assets with name (returns id)
    struct AssetRegistry
    {
        Map<u64, Pool>     pools;
        Map<ID, AssetInfo> id_to_info;
        String             extension = ".nit";
    };
    
    void SetAssetRegistryInstance(AssetRegistry* asset_registry_instance);
    AssetRegistry* GetAssetRegistryInstance();
    
    template<typename T>
    void RegisterAssetType(const AssetTypeArgs<T>& args)
    {
        AssetRegistry* asset_registry = GetAssetRegistryInstance();
        Pool& pool = asset_registry->pools[GetTypeHash<T>()];

        if (!IsTypeRegistered<T>())
        {
            RegisterType<T>();
        }
        
        Type* type = GetType<T>();
        SetLoadFunction(type, args.fn_load);
        SetFreeFunction(type, args.fn_free);
        SetSerializeFunction(type, args.fn_serialize);
        SetDeserializeFunction(type, args.fn_deserialize);
        
        InitPool<T>(&pool, args.max_elements);
    }

    bool IsAssetTypeRegistered(u64 type_hash);
    
    template<typename T>
    bool IsAssetTypeRegistered()
    {
        return IsAssetTypeRegistered(GetTypeHash<T>());
    }
    
    void BuildAssetPath(const String& name, String& path);
    void PushAssetInfo(const AssetInfo& asset_info, bool build_path);
    void EraseAssetInfo(ID id);
    
    ID DeserializeAssetFromString(const String& asset_str);
    ID DeserializeAssetFromFile(const String& file_path);
    void SerializeAssetToString(ID id, String& result);
    void SerializeAssetToFile(ID id);
    
    void InitAssetRegistry();
    
    Pool& GetAssetPool(u64 type_hash);
    
    Pool& GetAssetPool(const String& type_name);

    template<typename T>
    Pool& GetAssetPool()
    {
        return GetAssetPool(GetTypeHash<T>());
    }

    void FindAssetsByName(const String& name, Array<ID>& asset_ids);
    ID FindAssetByName(const String& name);
    
    template<typename T>
    T& GetAssetData(ID id)
    {
        Pool& pool = GetAssetPool<T>();
        return GetPoolElement<T>(&pool, id);
    }

    bool IsAssetValid(ID id);
    
    template<typename T>
    ID CreateAsset(const String& name, const String& path = "", const T& data = {})
    {
        Pool& pool = GetAssetPool<T>();
        ID id; InsertPoolElement(&pool, id, data);
        AssetInfo info { pool.type->name, name, path, id, 0, 0 };
        PushAssetInfo(info, true);
        return id;
    }

    void LoadAsset(ID id);
    void FreeAsset(ID id);
    void DestroyAsset(ID id);
}