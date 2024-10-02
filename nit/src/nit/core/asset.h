#pragma once

namespace Nit
{
    struct AssetData
    {
        void* load_func   = nullptr;
        void* unload_func = nullptr;
        void (*invoke_func)(void*, void*) = nullptr;
        Pool pool;
    };
    
    struct AssetRegistry
    {
        Array<AssetData> asset_data;
    };

    void SetAssetRegistryInstance(AssetRegistry* asset_registry_instance);

    AssetRegistry* GetAssetRegistryInstance();

    template<typename T>
    using AssetDataFunc = void (*) (T*); 
    
    template<typename T>
    void RegisterAssetType(
          u32              max_elements  = DEFAULT_POOL_ELEMENT_COUNT
        , AssetDataFunc<T> load_func     = nullptr
        , AssetDataFunc<T> unload_func   = nullptr
        )
    {
        AssetData& asset_data = GetAssetRegistryInstance()->asset_data.emplace_back(); 
        InitPool<T>(&asset_data.pool, max_elements);
        asset_data.load_func   = load_func;
        asset_data.unload_func = unload_func;
        
        asset_data.invoke_func = [](void* func, void* data) {
            AssetDataFunc<T> casted_func = static_cast<AssetDataFunc<T>>(func);
            T* casted_data = static_cast<T*>(data);
            casted_func(casted_data);
        };
    }

    void LoadAssets();
    
    void UnloadAssets();
    
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
    void DestroyAsset(ID id)
    {
        Pool& pool = GetAssetPool<T>();
        RemovePoolElement(&pool, id);
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