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

    bool IsAssetTypeRegistered(u64 type_hash)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        return asset_registry->pools.count(type_hash) != 0;
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

    void PushAssetInfo(AssetInfo& asset_info, bool build_path)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        NIT_CHECK(asset_info.id != 0);
        NIT_CHECK_MSG(asset_registry->id_to_info.count(asset_info.id) == 0, "Id already taken!");
        if (build_path)
        {
            BuildAssetPath(asset_info.name, asset_info.path);
        }
        asset_registry->id_to_info[asset_info.id] = asset_info;
    }
    
    void EraseAssetInfo(ID id)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        NIT_CHECK(id != 0);
        NIT_CHECK_MSG(asset_registry->id_to_info.count(id) != 0, "Id not registered!");
        asset_registry->id_to_info.erase(id);
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
                MappedPool& pool = GetAssetPool(asset_info.type_name);

                const bool created = asset_registry->id_to_info.count(asset_info.id) == 0; 
                
                if (created)
                {
                    PushAssetInfo(asset_info, false);
                    InsertDataWithID(&pool, asset_info.id);
                }

                void* data = GetRawData(pool.type, pool.elements, pool.element_id_to_index[asset_info.id]);
                DeserializeRawData(pool.type, data, asset_node);
                result_id = asset_info.id;

                if (created)
                {
                    AssetCreatedArgs args;
                    args.id   = asset_info.id;
                    args.type = pool.type;
                    Broadcast<const AssetCreatedArgs&>(GetAssetRegistryInstance()->asset_created_event, args);
                }
                
                if (IsAssetLoaded(asset_info.id))
                {
                    LoadAsset(result_id, true);
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

    void SerializeAssetToString(ID id, String& result)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        NIT_CHECK_MSG(asset_registry->id_to_info.count(id) != 0, "Id not registered!");
        AssetInfo& info = asset_registry->id_to_info[id];
        MappedPool& pool = GetAssetPool(info.type_name);

        YAML::Emitter emitter;
            
        emitter << YAML::BeginMap;
        emitter << YAML::Key << "AssetInfo" << YAML::Value << YAML::BeginMap;
        emitter << YAML::Key << "type_name" << YAML::Value << info.type_name;
        emitter << YAML::Key << "name"      << YAML::Value << info.name;
        emitter << YAML::Key << "path"      << YAML::Value << info.path;
        emitter << YAML::Key << "id"        << YAML::Value << info.id;
        emitter << YAML::Key << "version"   << YAML::Value << info.version;
        emitter << YAML::EndMap;
        
        emitter << YAML::Key << info.type_name << YAML::Value << YAML::BeginMap;
        SerializeRawData(pool.type, GetDataRaw(&pool, id), emitter);
        emitter << YAML::EndMap;

        result = emitter.c_str();
    }

    void SerializeAssetToFile(ID id)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        NIT_CHECK_MSG(asset_registry->id_to_info.count(id) != 0, "Id not registered!");
        AssetInfo& info = asset_registry->id_to_info[id];
        OutputFile file(info.path);
        
        if (file.is_open())
        {
            String asset_string;
            SerializeAssetToString(id, asset_string);
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

    MappedPool& GetAssetPool(u64 type_hash)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        NIT_CHECK_MSG(asset_registry->pools.count(type_hash), "Asset type not registered!");
        MappedPool& pool = asset_registry->pools[type_hash];
        return pool;
    }

    MappedPool& GetAssetPool(const String& type_name)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        Type* type = GetType(type_name);
        u64 hash = type->hash;
        NIT_CHECK(type && asset_registry->pools.count(hash));
        return asset_registry->pools[hash];
    }

    u32 GetLastAssetVersion(u64 type_hash)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        return asset_registry->hash_to_version.at(type_hash);
    }

    u32 GetLastAssetVersion(const String& type_name)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        return GetLastAssetVersion(GetType(type_name)->hash);
    }

    void FindAssetsByName(const String& name, Array<ID>& asset_ids)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        for (const auto& [id, info] : asset_registry->id_to_info)
        {
            if (info.name == name)
            {
                asset_ids.push_back(id);
            }
        }
    }

    ID FindAssetByName(const String& name)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        for (const auto& [id, info] : asset_registry->id_to_info)
        {
            if (info.name == name)
            {
                return id;
            }
        }
        return 0;
    }

    bool IsAssetValid(ID id)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        return id != 0 && asset_registry->id_to_info.count(id) != 0;
    }
    
    bool IsAssetLoaded(ID id)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        if (!IsAssetValid(id))
        {
            return false;
        }
        AssetInfo& info = asset_registry->id_to_info[id];
        return info.loaded;
    }

    void DestroyAsset(ID id)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        //TODO: Delete file?
        NIT_CHECK_MSG(asset_registry->id_to_info.count(id) != 0, "Id not registered!");
        AssetInfo& info = asset_registry->id_to_info[id];

        FreeAsset(id);

        MappedPool& pool = GetAssetPool(info.type_name);
        
        AssetDestroyedArgs args;
        args.id   = info.id;
        args.type = pool.type;
        Broadcast<const AssetDestroyedArgs&>(GetAssetRegistryInstance()->asset_destroyed_event, args);
        
        DeleteData(&pool, id);
        EraseAssetInfo(id);
    }

    void LoadAsset(ID id, bool force_reload)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        NIT_CHECK_MSG(asset_registry->id_to_info.count(id) != 0, "Id not registered!");
        AssetInfo& info = asset_registry->id_to_info[id];
        
        if (info.loaded)
        {
            if (force_reload)
            {
                // In this case we should keep the reference count.
                u32 reference_count = info.reference_count;
                FreeAsset(id);
                info.reference_count = reference_count;
            }
            else
            {
                return;
            }
        }
        
        info.loaded = true;
        MappedPool& pool = GetAssetPool(info.type_name);
        LoadRawData(pool.type, GetDataRaw(&pool, id));
    }

    void FreeAsset(ID id)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        NIT_CHECK_MSG(asset_registry->id_to_info.count(id) != 0, "Id not registered!");
        AssetInfo& info = asset_registry->id_to_info[id];
        
        if (!info.loaded)
        {
            return;
        }

        info.reference_count = 0;
        MappedPool& pool = GetAssetPool(info.type_name);
        FreeRawData(pool.type, GetDataRaw(&pool, id));
    }

    void RetainAsset(ID id)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED

        if (!IsAssetValid(id))
        {
            return;
        }
        
        AssetInfo& info = asset_registry->id_to_info[id];

        if (!info.loaded)
        {
            LoadAsset(id);
        }

        ++info.reference_count;
    }

    void ReleaseAsset(ID id, bool force_free)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED

        if (!IsAssetValid(id))
        {
            return;
        }
        
        AssetInfo& info = asset_registry->id_to_info[id];

        if (!info.loaded)
        {
            return;
        }

        // A loaded asset with reference_count of 0 is a valid case.
        if (force_free || info.reference_count <= 1)
        {
            FreeAsset(id);
            info.reference_count = 0;
            return;
        }
        
        --info.reference_count;
    }
}