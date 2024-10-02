#pragma once

namespace Nit
{
    template<typename T>
    using FnLoadAsset = void (*) (T*);

    template<typename T>
    using FnFreeAsset = FnLoadAsset<T>;
    
    struct AssetData
    {
        using FnInvokeLoad = void (*) (void*, void*);
        using FnInvokeFree = FnInvokeLoad;
        
        VoidFunc     fn_create      = nullptr;
        void*        fn_load        = nullptr;
        void*        fn_free        = nullptr;
        FnInvokeLoad fn_invoke_load = nullptr;
        FnInvokeFree fn_invoke_free = nullptr;
        Pool         pool;
    };
    
    struct AssetRegistry
    {
        Array<AssetData> asset_data;
    };

    void SetAssetRegistryInstance(AssetRegistry* asset_registry_instance);

    AssetRegistry* GetAssetRegistryInstance();
    
    template<typename T>
    Pair<ID, T&> CreateAsset(const T& data = {});
    
    template<typename T>
    void RegisterAssetType(
          u32            max_elements  = DEFAULT_POOL_ELEMENT_COUNT
        , FnLoadAsset<T> fn_load       = nullptr
        , FnFreeAsset<T> fn_free       = nullptr
        )
    {
        AssetData& asset_data = GetAssetRegistryInstance()->asset_data.emplace_back(); 
        InitPool<T>(&asset_data.pool, max_elements);
        
        asset_data.fn_load = fn_load;
        asset_data.fn_free = fn_free;
        
        asset_data.fn_create = [] {
            T empty_data;
            CreateAsset(empty_data);
        };
        
        asset_data.fn_invoke_load = [](void* fn, void* data) {
            FnLoadAsset<T> casted_fn = static_cast<FnLoadAsset<T>>(fn);
            T* casted_data = static_cast<T*>(data);
            casted_fn(casted_data);
        };
        
        asset_data.fn_invoke_free = asset_data.fn_invoke_load;
    }

    void LoadAssets();
    
    void FreeAssets();
    
    Pool* GetAssetPoolPtr(const char* asset_type);

    Pool& GetAssetPool(const char* asset_type);

    template<typename T>
    Pool& GetAssetPool()
    {
        const char* type_name = typeid(T).name();
        return GetAssetPool(type_name); 
    }

    template<typename T>
    Pair<ID, T&> CreateAsset(const T& data)
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