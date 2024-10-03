#pragma once

namespace Nit
{
    // using FnCreateAsset = void (*) (ID id, const String&);
    //
    // template<typename T>
    // using FnSerializeAsset = void (*) (const T*, YAML::Emitter& emitter);
    //
    // template<typename T>
    // using FnLoadAsset = void (*) (T*);
    //
    // template<typename T>
    // using FnFreeAsset = FnLoadAsset<T>;
    //
    // struct AssetPool
    // {
    //     using FnInvokeSerialize = void (*) (void*, const void*, YAML::Emitter& emitter); 
    //     using FnInvokeLoad      = void (*) (void*, void*);
    //     using FnInvokeFree      = FnInvokeLoad;
    //     
    //     const char*       extension;
    //     Map<String, ID>   name_to_id;
    //     Map<ID, String>   id_to_name;
    //     FnCreateAsset     fn_create            = nullptr;
    //     void*             fn_serialize         = nullptr;
    //     FnInvokeSerialize fn_invoke_serialize  = nullptr;
    //     void*             fn_load              = nullptr;
    //     void*             fn_free              = nullptr;
    //     FnInvokeLoad      fn_invoke_load       = nullptr;
    //     FnInvokeFree      fn_invoke_free       = nullptr;
    //     Pool              pool;
    // };
    //
    // struct AssetRegistry
    // {
    //     Array<AssetPool> asset_pools;
    // };
    //
    // void SetAssetRegistryInstance(AssetRegistry* asset_registry_instance);
    // AssetRegistry* GetAssetRegistryInstance();
    //
    // AssetPool* GetAssetPoolPtr(const char* asset_type);
    //
    // AssetPool& GetAssetPool(const char* asset_type);
    //
    // template<typename T>
    // AssetPool& GetAssetPool()
    // {
    //     const char* type_name = typeid(T).name();
    //     return GetAssetPool(type_name); 
    // }
    //
    // void PushAssetInfo(AssetPool& pool, ID id, const String& name);
    //
    // void PopAssetInfo(AssetPool& pool, ID id);
    //
    // template<typename T>
    // void RegisterAssetType(
    //       const char*         extension
    //     , FnSerializeAsset<T> fn_serialize
    //     , FnLoadAsset<T>      fn_load       = nullptr
    //     , FnFreeAsset<T>      fn_free       = nullptr
    //     , u32                 max_elements  = DEFAULT_POOL_ELEMENT_COUNT
    //     )
    // {
    //     AssetPool& asset_data = GetAssetRegistryInstance()->asset_pools.emplace_back(); 
    //     InitPool<T>(&asset_data.pool, max_elements);
    //     
    //     asset_data.fn_create = [] (ID id, const String& name) {
    //         AssetPool& pool = GetAssetPool<T>();
    //         T data;
    //         InsertPoolElementWithID(&pool.pool, id, data);
    //         PushAssetInfo(pool, id, name);
    //     };
    //     
    //     asset_data.extension = extension;
    //     asset_data.fn_serialize = fn_serialize;
    //     
    //     asset_data.fn_invoke_serialize = [] (void* fn, const void* data, YAML::Emitter& emitter) {
    //         FnSerializeAsset<T> casted_fn = static_cast<FnSerializeAsset<T>>(fn);
    //         const T* casted_data = static_cast<const T*>(data);
    //         casted_fn(casted_data, emitter);
    //     };
    //     
    //     asset_data.fn_load      = fn_load;
    //     asset_data.fn_free      = fn_free;
    //     
    //     asset_data.fn_invoke_load = [](void* fn, void* data) {
    //         FnLoadAsset<T> casted_fn = static_cast<FnLoadAsset<T>>(fn);
    //         T* casted_data = static_cast<T*>(data);
    //         casted_fn(casted_data);
    //     };
    //     
    //     asset_data.fn_invoke_free = asset_data.fn_invoke_load;
    // }
    //
    // void LoadAssets();
    //
    // void FreeAssets();
    //
    // void SerializeAsset(AssetPool& pool, ID id);
    //
    // Pair<ID, void*> CreateAsset(const String& name, void* data);
    //
    // template<typename T>
    // Pair<ID, T&> CreateAsset(const String& name, const T& data = {})
    // {
    //     AssetPool& pool = GetAssetPool<T>(); 
    //     ID id; InsertPoolElement(&pool.pool, id, data);
    //     PushAssetInfo(pool, id, name);
    //     T& asset_data = GetPoolElement<T>(&pool.pool, id);
    //     return { id, asset_data };
    // }
    //
    // template<typename T>
    // void DestroyAsset(ID id)
    // {
    //     AssetPool& pool = GetAssetPool<T>(); 
    //     PopAssetInfo(pool, id);
    //     RemovePoolElement(&pool.pool, id);
    // }
    //
    // template<typename T>
    // T& GetAsset(ID id)
    // {
    //     AssetPool& pool = GetAssetPool<T>();
    //     return GetPoolElement<T>(&pool.pool, id);
    // }
    //
    // template<typename T>
    // bool IsAssetValid(ID id)
    // {
    //     AssetPool& pool = GetAssetPool<T>();
    //     return IsPoolElementValid(&pool.pool, id);
    // }
}