#pragma once

namespace Nit
{
    struct AssetRegistry
    {
        Array<Pool> asset_data;
    };

    void SetAssetRegistryInstance(AssetRegistry* asset_registry_instance);

    AssetRegistry* GetAssetRegistryInstance();
    
    template<typename T>
    void RegisterAssetType(u32 max_elements = DEFAULT_POOL_ELEMENT_COUNT)
    {
        Pool& pool = GetAssetRegistryInstance()->asset_data.emplace_back();
        InitPool<T>(&pool, max_elements);
    }
    
    Pool* GetAssetPoolPtr(const char* asset_type);

    Pool& GetAssetPool(const char* asset_type);

    template<typename T>
    Pool& GetAssetPool()
    {
        const char* type_name = typeid(T).name();
        return GetAssetPool(type_name); 
    }

    template<typename T>
    Pair<ID, T&> CreateAsset(const T& data = {})
    {
        Pool& pool = GetAssetPool<T>(); 
        ID id; InsertPoolElement(&pool, id, data);
        T& asset_data = GetPoolElement<T>(&pool, id);
        return { id, asset_data };
    }
    
    template<typename T>
    T& GetAsset(ID id)
    {
        Pool& pool = GetAssetPool<T>();
        return GetPoolElement<T>(&pool, id);
    }

    template<typename T>
    bool IsAssetValid(ID id)
    {
        Pool& pool = GetAssetPool<T>();
        return IsPoolElementValid(&pool, id);
    }
}