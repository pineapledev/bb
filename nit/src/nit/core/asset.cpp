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

    bool IsAssetTypeRegistered(const String& type_name)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        return asset_registry->pools.count(type_name) != 0;
    }

    void BuildAssetPath(const String& name, String& path)
    {
        //TODO: sanity checks?
        path.append("\\").append(name).append(".").append(asset_registry->extension);
    }

    void PushAssetInfo(ID id, const String& name, const String& path, const String& type_name)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        NIT_CHECK_MSG(asset_registry->name_to_id.count(name) == 0, "Name already taken!");
        NIT_CHECK_MSG(asset_registry->id_to_info.count(id) == 0, "Id already taken!");
        asset_registry->name_to_id[name] = id;
        String final_path = path;
        BuildAssetPath(name, final_path);
        AssetInfo info = { type_name, id, name, final_path};
        asset_registry->id_to_info[id] = info;
    }
    
    void EraseAssetInfo(ID id)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        NIT_CHECK_MSG(asset_registry->id_to_info.count(id) != 0, "Id not registered!");
        String& name = asset_registry->id_to_info[id].name;
        NIT_CHECK_MSG(asset_registry->name_to_id.count(name) != 0, "Name not registered!");
        asset_registry->name_to_id.erase(name);
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
            asset_info.id = asset_info_node["id"].as<ID>();
            asset_info.name = asset_info_node["name"].as<String>();
            asset_info.path = asset_info_node["path"].as<String>();
                
            if (YAML::Node asset_node = node[asset_info.type_name])
            {
                Pool& pool = GetAssetPool(asset_info.type_name);

                if (!asset_registry->id_to_info.count(asset_info.id))
                {
                    PushAssetInfo(asset_info.id, asset_info.name, asset_info.path, asset_info.type_name);
                    InsertPoolElementRawWithID(&pool, asset_info.id);
                }

                void* data = GetRawData(pool.type, pool.elements, pool.element_id_to_index[asset_info.id]);
                DeserializeRawData(pool.type, data, asset_node);
                result_id = asset_info.id;
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
        Pool& pool = GetAssetPool(info.type_name);

        YAML::Emitter emitter;
            
        emitter << YAML::BeginMap;
        emitter << YAML::Key << "AssetInfo" << YAML::Value << YAML::BeginMap;
        emitter << YAML::Key << "type_name" << YAML::Value << info.type_name;
        emitter << YAML::Key << "id"        << YAML::Value << info.id;
        emitter << YAML::Key << "name"      << YAML::Value << info.name;
        emitter << YAML::Key << "path"      << YAML::Value << info.path;
        emitter << YAML::EndMap;
            
        emitter << YAML::BeginMap;
        emitter << YAML::Key << info.type_name << YAML::Value << YAML::BeginMap;
        SerializeRawData(pool.type, GetPoolElementRawPtr(&pool, id), emitter);
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
            
            if (dir_entry.is_directory() || dir_path.extension() != asset_registry->extension)
            {
                continue;
            }

            DeserializeAssetFromFile(dir_path.string());
        }
    }

    Pool& GetAssetPool(const String& type_name)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        NIT_CHECK_MSG(asset_registry->pools.count(type_name), "Asset type not registered!");
        Pool& pool = asset_registry->pools[type_name];
        return pool;
    }

    void DestroyAsset(ID id)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        //TODO: Delete file?
        NIT_CHECK_MSG(asset_registry->id_to_info.count(id) != 0, "Id not registered!");
        AssetInfo& info = asset_registry->id_to_info[id];
        Pool& pool = GetAssetPool(info.type_name);
        RemovePoolElement(&pool, id);
        EraseAssetInfo(id);
    }

    void LoadAsset(ID id)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        NIT_CHECK_MSG(asset_registry->id_to_info.count(id) != 0, "Id not registered!");
        AssetInfo& info = asset_registry->id_to_info[id];
        Pool& pool = GetAssetPool(info.type_name);
        LoadRawData(pool.type, GetPoolElementRawPtr(&pool, id));
    }

    void FreeAsset(ID id)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        NIT_CHECK_MSG(asset_registry->id_to_info.count(id) != 0, "Id not registered!");
        AssetInfo& info = asset_registry->id_to_info[id];
        Pool& pool = GetAssetPool(info.type_name);
        FreeRawData(pool.type, GetPoolElementRawPtr(&pool, id));
    }
}