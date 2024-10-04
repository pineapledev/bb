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

    void PushAssetInfo(ID id, const String& name, const String& path, const String& type_name)
    {
        NIT_CHECK_MSG(asset_registry->name_to_id.count(name) == 0, "Name already taken!");
        NIT_CHECK_MSG(asset_registry->id_to_info.count(id) == 0, "Id already taken!");
        asset_registry->name_to_id[name] = id;
        AssetInfo info = { type_name, id, name, path};
        asset_registry->id_to_info[id] = info;
    }
    
    void EraseAssetInfo(ID id)
    {
        NIT_CHECK_MSG(asset_registry->id_to_info.count(id) != 0, "Id not registered!");
        String& name = asset_registry->id_to_info[id].name;
        NIT_CHECK_MSG(asset_registry->name_to_id.count(name) != 0, "Name not registered!");
        asset_registry->name_to_id.erase(name);
        asset_registry->id_to_info.erase(id);
    }

    Pool& GetAssetPool(const String& type_name)
    {
        NIT_CHECK_MSG(asset_registry->pools.count(type_name), "Asset type not registered!");
        Pool& pool = asset_registry->pools[type_name];
        return pool;
    }

    void DestroyAsset(ID id)
    {
        //TODO: Delete file?
        NIT_CHECK_MSG(asset_registry->id_to_info.count(id) != 0, "Id not registered!");
        AssetInfo& info = asset_registry->id_to_info[id];
        Pool& pool = GetAssetPool(info.type_name);
        RemovePoolElement(&pool, id);
        EraseAssetInfo(id);
    }

    void DeserializeAssets()
    {
        for (const auto& dir_entry : RecursiveDirectoryIterator(GetWorkingDirectory()))
        {
            const Path& dir_path = dir_entry.path();
            
            if (dir_entry.is_directory() || dir_path.extension() != asset_registry->extension) 
                continue;

            YAML::Node node = YAML::LoadFile(dir_path.string());

            if (YAML::Node asset_info_node = node["AssetInfo"])
            {
                AssetInfo asset_info;
                asset_info.type_name = asset_info_node["type_name"].as<String>();
                asset_info.id = asset_info_node["id"].as<ID>();
                asset_info.name = asset_info_node["name"].as<String>();
                asset_info.path = asset_info_node["path"].as<String>();
                
                if (YAML::Node asset_node = node[asset_info.type_name])
                {
                    Pool& pool = GetAssetPool(asset_info.type_name);
                    InsertPoolElementRawWithID(&pool, asset_info.id);
                    PushAssetInfo(asset_info.id, asset_info.name, asset_info.path, asset_info.type_name);
                }
            }
        }
    }

    // void DeserializeAsset(const AssetInfo& asset_info)
    // {
    //     AssetPool& pool = GetAssetPool(asset_info.type_name);
    //     YAML::Node node = YAML::LoadFile(asset_info.path);
    //     
    //     void* data = nullptr;
    //
    //     // Asset is created
    //     if (asset_registry->id_to_info.count(asset_info.id))
    //     {
    //         DeserializeRawData(pool.data_pool.type, data, node);
    //         u32 element_index = pool.data_pool.element_id_to_index[asset_info.id];
    //         SetRawData(pool.data_pool.type, pool.data_pool.elements, element_index, data);
    //         return;
    //     }
    //
    //     // Asset is not created
    //     //InsertPoolElementRawWithID(pool.data_pool)
    // }
    //
    // void LoadAssets()
    // {
    //     for (auto& [type_name, pool] : asset_registry->pools)
    //     {
    //         Pool& data_pool = pool.data_pool;
    //         Type* type = data_pool.type;
    //         
    //         if (type->fn_load)
    //         {
    //             for (u32 i = 0; i < data_pool.count; ++i)
    //             {
    //                 LoadRawData(type, GetRawData(type, data_pool.elements, i));
    //             }
    //         }
    //     }
    // }
    //
    // void FreeAssets()
    // {
    //     for (auto& [type_name, pool] : asset_registry->pools)
    //     {
    //         Pool& data_pool = pool.data_pool;
    //         Type* type = data_pool.type;
    //         
    //         if (type->fn_free)
    //         {
    //             for (u32 i = 0; i < data_pool.count; ++i)
    //             {
    //                 FreeRawData(type, GetRawData(type, data_pool.elements, i));
    //             }
    //         }
    //     }
    // }
    
    // void SerializeAsset(AssetPool& pool, ID id)
    // {
    //     NIT_CHECK_MSG(IsPoolElementValid(&pool.data_pool, id), "Asset not registered!");
    //     String& asset_name = pool.id_to_name[id];
    //     OutputFile output_file(asset_name.append(".").append(pool.extension));
    //     NIT_CHECK_MSG(output_file.is_open(), "File should be opened!");
    //     YAML::Emitter emitter;
    //     emitter << YAML::BeginMap;
    //     emitter << YAML::Key << pool.data_pool.type_name << YAML::Value << YAML::BeginMap;
    //     emitter << YAML::Key << "id" << YAML::Value << id;
    //     Pool& data_pool = pool.data_pool;
    //     void* data = data_pool.get_data(data_pool.elements, data_pool.element_id_to_index[id]);
    //     pool.fn_invoke_serialize(pool.fn_serialize, data, emitter);
    //     output_file << emitter.c_str();
    //     output_file.flush();
    //     output_file.close();
    // }
}
