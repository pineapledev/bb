#include "asset.h"

#define NIT_CHECK_ASSET_REGISTRY_CREATED NIT_CHECK_MSG(asset_registry, "Forget to call SetRenderer2DInstance!");

namespace Nit
{
    AssetRegistry* asset_registry = nullptr;
    
    void SetAssetRegistryInstance(AssetRegistry* asset_registry_instance)
    {
        NIT_CHECK(asset_registry_instance);
        asset_registry = asset_registry_instance;
    }

    AssetRegistry* GetAssetRegistryInstance()
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        return asset_registry;
    }

    void LoadAssets()
    {
        for (AssetData& asset_data : GetAssetRegistryInstance()->asset_data)
        {
            if (!asset_data.load_func)
            {
                continue;
            }
            
            NIT_CHECK_MSG(asset_data.invoke_func, "Invoke func not registered!")
            for (u32 i = 0; i < asset_data.pool.count; ++i)
            {
                void* element = asset_data.pool.get_data(asset_data.pool.elements, i);
                asset_data.invoke_func(asset_data.load_func, element);
            }
        }
    }

    void UnloadAssets()
    {
        for (AssetData& asset_data : GetAssetRegistryInstance()->asset_data)
        {
            if (!asset_data.unload_func)
            {
                continue;
            }
            
            NIT_CHECK_MSG(asset_data.invoke_func, "Invoke func not registered!")
            for (u32 i = 0; i < asset_data.pool.count; ++i)
            {
                void* element = asset_data.pool.get_data(asset_data.pool.elements, i);
                asset_data.invoke_func(asset_data.unload_func, element);
            }
        }
    }

    Pool* GetAssetPoolPtr(const char* asset_type)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        
        for (u32 i = 0; i < asset_registry->asset_data.size(); ++i)
        {
            Pool* pool = &asset_registry->asset_data[i].pool;
            if (pool->type_name == asset_type)
            {
                return pool;
            }
        }
        
        return nullptr;
    }

    Pool& GetAssetPool(const char* asset_type)
    {
        if (Pool* pool = GetAssetPoolPtr(asset_type))
        {
            return *pool;
        }
        
        NIT_CHECK_MSG(false, "Invalid asset type!");
        static Pool default_pool;
        return default_pool;
    }
}
