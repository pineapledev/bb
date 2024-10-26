#include "asset.h"

#define NIT_CHECK_ASSET_REGISTRY_CREATED NIT_CHECK_MSG(asset_registry, "Forget to call SetAssetRegistryInstance!");

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

    AssetPool* GetAssetPool(Type* type)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        
        Array<AssetPool>* assets = &asset_registry->asset_pools;
        
        auto it = std::ranges::find_if(*assets, [&type](AssetPool& asset){
            return asset.data_pool.type == type;
        });
        
        if (it == assets->end())
        {
            return nullptr;
        }
        
        return &(*it);
    }

    bool IsAssetTypeRegistered(Type* type)
    {
        return GetAssetPool(type) != nullptr;
    }

    void BuildAssetPath(const String& name, String& path)
    {
        //TODO: sanity checks?
        if (!path.empty())
        {
            path.append("\\");    
        }
        path.append(name).append(asset_registry->extension);
    }

    void PushAssetInfo(AssetInfo& asset_info, u32 index, bool build_path)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        NIT_CHECK(asset_info.id != 0);
        
        AssetPool* pool = GetAssetPool(GetType(asset_info.type_name));
        NIT_CHECK(pool);
        
        if (build_path)
        {
            BuildAssetPath(asset_info.name, asset_info.path);
        }
        
        pool->asset_infos[index] = asset_info;
    }
    
    void EraseAssetInfo(AssetInfo& asset_info, SparseSetDeletion deletion)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        NIT_CHECK(asset_info.id != 0);
        AssetPool* pool = GetAssetPool(GetType(asset_info.type_name));
        NIT_CHECK(pool);
        pool->asset_infos[deletion.deleted_slot] = pool->asset_infos[deletion.last_slot];
    }

    AssetInfo* GetAssetInfo(Type* type, ID id)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        NIT_CHECK(id != 0);
        AssetPool* pool = GetAssetPool(type);
        if (!pool)
        {
            NIT_CHECK_MSG(false, "Trying to get the asset info from non registered asset type");
            return nullptr;
        }

        if (!IsValid(&pool->data_pool, id))
        {
            return nullptr;
        }
        
        return &pool->asset_infos[IndexOf(&pool->data_pool, id)];
    }

    ID DeserializeAssetFromString(const String& asset_str)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        ID result_id = 0;
        YAML::Node node = YAML::Load(asset_str);

        if (YAML::Node asset_info_node = node["AssetInfo"])
        {
            AssetInfo asset_info;
            asset_info.type_name = asset_info_node["type_name"].as<String>();
            asset_info.name = asset_info_node["name"].as<String>();
            asset_info.path = asset_info_node["path"].as<String>();
            asset_info.id = asset_info_node["id"].as<ID>();
            asset_info.version = asset_info_node["version"].as<u32>();
            
            if (asset_info.version < GetLastAssetVersion(asset_info.type_name))
            {
                NIT_CHECK_MSG(false, "Trying to load an outdated asset, please upgrade the current asset version!!");
                return result_id;
            }
            
            if (YAML::Node asset_node = node[asset_info.type_name])
            {
                AssetPool* pool = GetAssetPool(GetType(asset_info.type_name));
                
                NIT_CHECK_MSG(pool, "Trying to deserialize an unregistered type of asset!");
                
                const bool created = IsValid(&pool->data_pool, asset_info.id);
                
                if (!created)
                {
                    InsertDataWithID(&pool->data_pool, asset_info.id);
                    PushAssetInfo(asset_info, IndexOf(&pool->data_pool, asset_info.id), false);
                }
                
                void* data = GetDataRaw(&pool->data_pool, asset_info.id);
                Deserialize(pool->data_pool.type, data, asset_node);
                result_id = asset_info.id;

                if (created)
                {
                    AssetCreatedArgs args;
                    args.id   = asset_info.id;
                    args.type = pool->data_pool.type;
                    Broadcast<const AssetCreatedArgs&>(GetAssetRegistryInstance()->asset_created_event, args);
                }
                
                if (IsAssetLoaded(pool->data_pool.type, asset_info.id))
                {
                    LoadAsset(pool->data_pool.type, result_id, true);
                }
            }
        }
        
        return result_id;
    }

    ID DeserializeAssetFromFile(const String& file_path)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        InputFile input_file(file_path);

        if (input_file.is_open())
        {
            StringStream stream;
            stream << input_file.rdbuf();
            return DeserializeAssetFromString(stream.str());
        }
        
        NIT_CHECK_MSG(false, "Cannot open file!");
        return 0;
    }

    void SerializeAssetToString(Type* type, ID id, String& result)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        AssetPool* pool = GetAssetPool(type);

        if (!pool)
        {
            NIT_CHECK_MSG(false, "Trying to serialize an unregistered type of asset!");
            return;
        }
        
        const AssetInfo* info = GetAssetInfo(type, id);

        if (!info)
        {
            NIT_CHECK_MSG(false, "Trying to serialize an invalid asset");
            return;
        }
        
        YAML::Emitter emitter;
            
        emitter << YAML::BeginMap;
        emitter << YAML::Key << "AssetInfo" << YAML::Value << YAML::BeginMap;
        emitter << YAML::Key << "type_name" << YAML::Value << info->type_name;
        emitter << YAML::Key << "name"      << YAML::Value << info->name;
        emitter << YAML::Key << "path"      << YAML::Value << info->path;
        emitter << YAML::Key << "id"        << YAML::Value << info->id;
        emitter << YAML::Key << "version"   << YAML::Value << info->version;
        emitter << YAML::EndMap;
        
        emitter << YAML::Key << info->type_name << YAML::Value << YAML::BeginMap;
        
        Serialize(pool->data_pool.type, GetDataRaw(&pool->data_pool, id), emitter);

        emitter << YAML::EndMap;

        result = emitter.c_str();
    }

    void SerializeAssetToFile(Type* type, ID id)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED

        const AssetInfo* info = GetAssetInfo(type, id);

        if (!info)
        {
            NIT_CHECK_MSG(false, "Trying to serialize an invalid asset");
            return;
        }
        
        OutputFile file(info->path);
        
        if (file.is_open())
        {
            String asset_string;
            SerializeAssetToString(type, id, asset_string);
            file << asset_string;
            file.flush();
            file.close();
            return;
        }
        
        NIT_CHECK_MSG(false, "Cannot open file!");
    }

    void InitAssetRegistry()
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        for (const auto& dir_entry : RecursiveDirectoryIterator(GetWorkingDirectory()))
        {
            const Path& dir_path = dir_entry.path();
            
            if (dir_entry.is_directory() || dir_path.extension().string() != asset_registry->extension)
            {
                continue;
            }

            DeserializeAssetFromFile(dir_path.string());
        }
    }

    u32 GetLastAssetVersion(Type* type)
    {
        AssetPool* pool = GetAssetPool(type);

        if (!pool)
        {
            NIT_CHECK(false);
            return 0;
        }

        return pool->version;
    }

    u32 GetLastAssetVersion(const String& type_name)
    {
        return GetLastAssetVersion(GetType(type_name));
    }

    void FindAssetsByName(const String& name, Array<ID>& asset_ids)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED

        for (AssetPool& asset_pool : asset_registry->asset_pools)
        {
            for (u32 i = 0; i < asset_pool.data_pool.sparse_set.count; ++i)
            {
                AssetInfo* asset_info = &asset_pool.asset_infos[i];
                if (asset_info->name == name)
                {
                    asset_ids.push_back(asset_info->id);
                }
            }
        }
    }

    ID FindAssetByName(const String& name)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED

        for (AssetPool& asset_pool : asset_registry->asset_pools)
        {
            for (u32 i = 0; i < asset_pool.data_pool.sparse_set.count; ++i)
            {
                AssetInfo* asset_info = &asset_pool.asset_infos[i];
                if (asset_info->name == name)
                {
                    return asset_info->id;
                }
            }
        }
        
        return 0;
    }

    bool IsAssetValid(Type* type, ID id)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED

        AssetPool* pool = GetAssetPool(type);

        if (!pool)
        {
            NIT_CHECK(false);
            return false;
        }
        
        return IsValid(&pool->data_pool, id);
    }
    
    bool IsAssetLoaded(Type* type, ID id)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED

        if (!IsAssetValid(type, id))
        {
            return false;
        }
        
        const AssetInfo* info = GetAssetInfo(type, id);

        if (!info)
        {
            NIT_CHECK_MSG(false, "Invalid asset");
            return false;
        }
        
        return info->loaded;
    }

    void DestroyAsset(Type* type, ID id)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        //TODO: Delete file?
        
        AssetInfo* info = GetAssetInfo(type, id);

        if (!info)
        {
            NIT_CHECK_MSG(false, "Trying to serialize an invalid asset");
            return;
        }
        
        FreeAsset(type, id);

        AssetPool* pool = GetAssetPool(type);

        if (!pool)
        {
            NIT_CHECK(false);
            return;
        }
        
        AssetDestroyedArgs args;
        args.id   = info->id;
        args.type = type;
        Broadcast<const AssetDestroyedArgs&>(GetAssetRegistryInstance()->asset_destroyed_event, args);
        
        EraseAssetInfo(*info, DeleteData(&pool->data_pool, id));
    }

    void LoadAsset(Type* type, ID id, bool force_reload)
    {
        AssetInfo* info = GetAssetInfo(type, id);

        if (!info)
        {
            NIT_CHECK_MSG(false, "Trying to serialize an invalid asset");
            return;
        }
        
        if (info->loaded)
        {
            if (force_reload)
            {
                // In this case we should keep the reference count.
                u32 reference_count = info->reference_count;
                FreeAsset(type, info->id);
                info->reference_count = reference_count;
            }
            else
            {
                return;
            }
        }
        
        info->loaded = true;

        AssetPool* pool = GetAssetPool(type);

        if (!pool)
        {
            NIT_CHECK(false);
            return;
        }
        
        Load(type, GetDataRaw(&pool->data_pool, id));
    }

    void FreeAsset(Type* type, ID id)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED

        AssetInfo* info = GetAssetInfo(type, id);

        if (!info)
        {
            NIT_CHECK_MSG(false, "Trying to serialize an invalid asset");
            return;
        }
        
        if (!info->loaded)
        {
            return;
        }

        info->reference_count = 0;

        AssetPool* pool = GetAssetPool(type);

        if (!pool)
        {
            NIT_CHECK(false);
            return;
        }
        
        Free(type, GetDataRaw(&pool->data_pool, id));
    }

    void RetainAsset(Type* type, ID id)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED

        if (!IsAssetValid(type, id))
        {
            return;
        }
        
        AssetInfo* info = GetAssetInfo(type, id);

        if (!info)
        {
            NIT_CHECK_MSG(false, "Trying to serialize an invalid asset");
            return;
        }

        if (!info->loaded)
        {
            LoadAsset(type, id);
        }

        ++info->reference_count;
    }

    void ReleaseAsset(Type* type, ID id, bool force_free)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED

        if (!IsAssetValid(type, id))
        {
            return;
        }
        
        AssetInfo* info = GetAssetInfo(type, id);

        if (!info)
        {
            NIT_CHECK_MSG(false, "Trying to serialize an invalid asset");
            return;
        }

        if (!info->loaded)
        {
            return;
        }

        // A loaded asset with reference_count of 0 is a valid case.
        if (force_free || info->reference_count <= 1)
        {
            FreeAsset(type, id);
            info->reference_count = 0;
            return;
        }
        
        --info->reference_count;
    }
}