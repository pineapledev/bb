#include "asset.h"

#define NIT_CHECK_ASSET_REGISTRY_CREATED NIT_CHECK_MSG(asset_registry, "Forget to call SetRenderer2DInstance!");

namespace Nit
{
    // AssetRegistry* asset_registry = nullptr;
    //
    // void SetAssetRegistryInstance(AssetRegistry* asset_registry_instance)
    // {
    //     NIT_CHECK(asset_registry_instance);
    //     asset_registry = asset_registry_instance;
    // }
    //
    // AssetRegistry* GetAssetRegistryInstance()
    // {
    //     NIT_CHECK_ASSET_REGISTRY_CREATED
    //     return asset_registry;
    // }
    //
    // void LoadAssets()
    // {
    //     for (AssetPool& asset_pool : GetAssetRegistryInstance()->asset_pools)
    //     {
    //         if (!asset_pool.fn_load)
    //         {
    //             continue;
    //         }
    //         
    //         NIT_CHECK_MSG(asset_pool.fn_invoke_load, "Invoke func not registered!")
    //         for (u32 i = 0; i < asset_pool.pool.count; ++i)
    //         {
    //             void* element = asset_pool.pool.get_data(asset_pool.pool.elements, i);
    //             asset_pool.fn_invoke_load(asset_pool.fn_load, element);
    //         }
    //     }
    // }
    //
    // void FreeAssets()
    // {
    //     for (AssetPool& asset_pool : GetAssetRegistryInstance()->asset_pools)
    //     {
    //         if (!asset_pool.fn_free)
    //         {
    //             continue;
    //         }
    //         
    //         NIT_CHECK_MSG(asset_pool.fn_invoke_free, "Invoke func not registered!")
    //         for (u32 i = 0; i < asset_pool.pool.count; ++i)
    //         {
    //             void* element = asset_pool.pool.get_data(asset_pool.pool.elements, i);
    //             asset_pool.fn_invoke_free(asset_pool.fn_free, element);
    //         }
    //     }
    // }
    //
    // AssetPool* GetAssetPoolPtr(const char* asset_type)
    // {
    //     NIT_CHECK_ASSET_REGISTRY_CREATED
    //     
    //     for (u32 i = 0; i < asset_registry->asset_pools.size(); ++i)
    //     {
    //         AssetPool* pool = &asset_registry->asset_pools[i];
    //         if (pool->pool.type_name == asset_type)
    //         {
    //             return pool;
    //         }
    //     }
    //     
    //     return nullptr;
    // }
    //
    // AssetPool& GetAssetPool(const char* asset_type)
    // {
    //     if (AssetPool* pool = GetAssetPoolPtr(asset_type))
    //     {
    //         return *pool;
    //     }
    //     
    //     NIT_CHECK_MSG(false, "Invalid asset type!");
    //     static AssetPool default_pool;
    //     return default_pool;
    // }
    //
    // void PushAssetInfo(AssetPool& pool, ID id, const String& name)
    // {
    //     NIT_CHECK_MSG(IsPoolElementValid(&pool.pool, id), "Asset not registered!");
    //     NIT_CHECK_MSG(pool.name_to_id.count(name) == 0, "Name already taken!");
    //     NIT_CHECK_MSG(pool.id_to_name.count(id) == 0, "Id already taken!");
    //     pool.name_to_id[name] = id;
    //     pool.id_to_name[id]   = name;
    // }
    //
    // void PopAssetInfo(AssetPool& pool, ID id)
    // {
    //     NIT_CHECK_MSG(IsPoolElementValid(&pool.pool, id), "Asset not registered!");
    //     NIT_CHECK_MSG(pool.id_to_name.count(id) != 0, "Id not registered!");
    //     String& name = pool.id_to_name[id];
    //     NIT_CHECK_MSG(pool.name_to_id.count(name) != 0, "Name not registered!");
    //     pool.name_to_id.erase(name);
    //     pool.id_to_name.erase(id);
    // }
    //
    // void SerializeAsset(AssetPool& pool, ID id)
    // {
    //     NIT_CHECK_MSG(IsPoolElementValid(&pool.pool, id), "Asset not registered!");
    //     String& asset_name = pool.id_to_name[id];
    //     OutputFile output_file(asset_name.append(".").append(pool.extension));
    //     NIT_CHECK_MSG(output_file.is_open(), "File should be opened!");
    //     YAML::Emitter emitter;
    //     emitter << YAML::BeginMap;
    //     emitter << YAML::Key << pool.pool.type_name << YAML::Value << YAML::BeginMap;
    //     emitter << YAML::Key << "id" << YAML::Value << id;
    //     Pool& data_pool = pool.pool;
    //     void* data = data_pool.get_data(data_pool.elements, data_pool.element_id_to_index[id]);
    //     pool.fn_invoke_serialize(pool.fn_serialize, data, emitter);
    //     output_file << emitter.c_str();
    //     output_file.flush();
    //     output_file.close();
    // }
    //
    // Pair<ID, void*> CreateAsset(const String& name, void* data)
    // {
    //     
    // }
}
