#include "asset.h"

#define NIT_CHECK_ASSET_REGISTRY_CREATED NIT_CHECK_MSG(asset_registry, "Forget to call SetAssetRegistryInstance!");

namespace nit
{
    AssetRegistry* asset_registry = nullptr;

    AssetHandle create_asset_handle(AssetInfo* asset_info)
    {
        AssetHandle asset_handle;

        if (!asset_info)
        {
            return asset_handle;
        }
        
        if (!IsValid(asset_info->id) || asset_info->type == nullptr)
        {
            return asset_handle;
        }

        asset_handle.id      = asset_info->id;
        asset_handle.name    = asset_info->name;
        asset_handle.type    = asset_info->type;
        asset_handle.data_id = asset_info->data_id;
        return asset_handle;
    }

    void retarget_asset_handle(AssetHandle& asset_handle)
    {
        if (!IsValid(asset_handle.id) || asset_handle.type == nullptr)
        {
            return;
        }

        if (asset_handle.data_id == SparseSet::INVALID && asset_registry->id_to_data_id.count(asset_handle.id) != 0)
        {
            asset_handle.data_id = asset_registry->id_to_data_id.at(asset_handle.id);
        }
        
        AssetInfo* asset_info = get_asset_info(asset_handle);

        if (!asset_info)
        {
            return;
        }

        asset_handle = create_asset_handle(asset_info);
    }

    Path get_assets_directory()
    {
        return GetWorkingDirectory().string().append("\\assets");
    }

    void set_asset_registry_instance(AssetRegistry* asset_registry_instance)
    {
        NIT_CHECK(asset_registry_instance);
        asset_registry = asset_registry_instance;
    }
    
    AssetRegistry* get_asset_registry_instance()
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        return asset_registry;
    }

    AssetPool* get_asset_pool(Type* type)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED

        if (!type)
        {
            return nullptr;
        }
        
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

    AssetPool* get_asset_pool_safe(Type* type)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        AssetPool* pool = get_asset_pool(type);
        if (!pool)
        {
            NIT_CHECK_MSG(false, "Trying to get the asset pool from non registered asset type");
        }
        return pool;
    }

    AssetPool* GetAssetPoolSafe(AssetHandle& asset)
    {
        NIT_CHECK(asset.id.data != 0);
        return get_asset_pool_safe(asset.type);
    }

    AssetPool* GetAssetPoolSafe(const AssetInfo& info)
    {
        NIT_CHECK(info.id.data != 0);
        return get_asset_pool_safe(info.type);
    }

    bool is_asset_type_registered(Type* type)
    {
        return get_asset_pool(type) != nullptr;
    }

    void build_asset_path(const String& name, String& path)
    {
        //TODO: sanity checks?
        if (!path.empty())
        {
            path.append("\\");    
        }
        path.append(name).append(asset_registry->extension);
    }

    void push_asset_info(AssetInfo& asset_info, u32 index, bool build_path)
    {
        if (index == SparseSet::INVALID)
        {
            return;
        }
        
        AssetPool* pool = GetAssetPoolSafe(asset_info);
        
        if (build_path)
        {
            build_asset_path(asset_info.name, asset_info.path);
        }
        
        pool->asset_infos[index] = asset_info;
    }
    
    void erase_asset_info(AssetInfo& asset_info, SparseSetDeletion deletion)
    {
        AssetPool* pool = GetAssetPoolSafe(asset_info);
        pool->asset_infos[deletion.deleted_slot] = pool->asset_infos[deletion.last_slot];
    }

    AssetInfo* get_asset_info(AssetHandle& asset)
    {
        AssetPool* pool = GetAssetPoolSafe(asset);

        if (!pool::is_valid(&pool->data_pool, asset.data_id))
        {
            return nullptr;
        }
        
        return &pool->asset_infos[ pool::index_of(&pool->data_pool, asset.data_id)];
    }

    AssetInfo* GetAssetInfoSafe(AssetHandle& asset)
    {
        AssetInfo* info = get_asset_info(asset);
        if (!info)
        {
            NIT_CHECK_MSG(false, "Trying to get the asset info from non registered asset type");
        }
        return info;
    }

    AssetHandle deserialize_asset_from_string(const String& asset_str)
    {
        AssetHandle result;
        YAML::Node node = YAML::Load(asset_str);
        
        if (YAML::Node asset_info_node = node["AssetInfo"])
        {
            AssetInfo asset_info;
            asset_info.type    = GetType(asset_info_node["type"].as<String>());
            asset_info.name    = asset_info_node["name"].as<String>();
            asset_info.path    = asset_info_node["path"].as<String>();
            asset_info.id      = { static_cast<UUID>(asset_info_node["id"].as<nit::u64>()) };
            asset_info.version = asset_info_node["version"].as<u32>();
            
            if (asset_info.version < get_last_asset_version(asset_info.type))
            {
                NIT_CHECK_MSG(false, "Trying to load an outdated asset, please upgrade the current asset version!!");
                return result;
            }
            
            if (YAML::Node asset_node = node[asset_info.type->name])
            {
                AssetPool* pool = get_asset_pool(asset_info.type);
                
                NIT_CHECK_MSG(pool, "Trying to deserialize an unregistered type of asset!");

                bool created = asset_registry->id_to_data_id.count(asset_info.id) != 0;
                
                if (!created)
                {
                    pool::insert_data(&pool->data_pool, asset_info.data_id);
                    push_asset_info(asset_info,  pool::index_of(&pool->data_pool, asset_info.data_id), false);
                }
                else
                {
                    asset_info.data_id = asset_registry->id_to_data_id.at(asset_info.id);
                }
                
                result = create_asset_handle(&asset_info);
                void* data = pool::get_raw_data(&pool->data_pool, result.data_id);

                if (!created)
                {
                    asset_registry->id_to_data_id.insert({asset_info.id, asset_info.data_id});
                }
                
                Deserialize(pool->data_pool.type, data, asset_node);
                
                if (created)
                {
                    broadcast<const AssetCreatedArgs&>(get_asset_registry_instance()->asset_created_event, {result});
                }
            }
        }
        
        return result;
    }

    AssetHandle deserialize_asset_from_file(const String& file_path)
    {
        String path = "assets/" + file_path;
        InputFile input_file(path);

        if (input_file.is_open())
        {
            StringStream stream;
            stream << input_file.rdbuf();
            return deserialize_asset_from_string(stream.str());
        }
        
        NIT_CHECK_MSG(false, "Cannot open file!");
        return {};
    }

    void serialize_asset_to_string(AssetHandle& asset, String& result)
    {
        AssetPool* pool = GetAssetPoolSafe(asset);
        AssetInfo* info = GetAssetInfoSafe(asset);

        YAML::Emitter emitter;
            
        emitter << YAML::BeginMap;
        emitter << YAML::Key << "AssetInfo" << YAML::Value << YAML::BeginMap;
        emitter << YAML::Key << "type"      << YAML::Value << info->type->name;
        emitter << YAML::Key << "name"      << YAML::Value << info->name;
        emitter << YAML::Key << "path"      << YAML::Value << info->path;
        emitter << YAML::Key << "id"        << YAML::Value << (u64) info->id;
        emitter << YAML::Key << "version"   << YAML::Value << info->version;
        emitter << YAML::EndMap;
        
        emitter << YAML::Key << info->type->name << YAML::Value << YAML::BeginMap;

        Serialize(pool->data_pool.type, pool::get_raw_data(&pool->data_pool, info->data_id), emitter);

        emitter << YAML::EndMap;

        result = emitter.c_str();
    }

    void serialize_asset_to_file(AssetHandle& asset)
    {
        AssetInfo* info = GetAssetInfoSafe(asset);
        String path = "assets/" + info->path;
        
        OutputFile file(path);
        
        if (file.is_open())
        {
            String asset_string;
            serialize_asset_to_string(asset, asset_string);
            file << asset_string;
            file.flush();
            file.close();
            return;
        }
        
        NIT_CHECK_MSG(false, "Cannot open file!");
    }

    void init_asset_registry()
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        
        for (const auto& dir_entry : RecursiveDirectoryIterator(get_assets_directory()))
        {
            const Path& dir_path = std::filesystem::relative(dir_entry.path(), get_assets_directory());
            
            if (dir_entry.is_directory() || dir_path.extension().string() != asset_registry->extension)
            {
                continue;
            }

            deserialize_asset_from_file(dir_path.string());
        }
    }

    u32 get_last_asset_version(Type* type)
    {
        return get_asset_pool_safe(type)->version;
    }

    u32 get_last_asset_version(const String& type_name)
    {
        return get_last_asset_version(GetType(type_name));
    }

    void find_assets_by_name(const String& name, Array<AssetHandle>& assets)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED

        for (AssetPool& asset_pool : asset_registry->asset_pools)
        {
            for (u32 i = 0; i < asset_pool.data_pool.sparse_set.count; ++i)
            {
                AssetInfo* asset_info = &asset_pool.asset_infos[i];
                if (asset_info->name == name)
                {
                    AssetHandle handle;
                    assets.push_back(create_asset_handle(asset_info));
                }
            }
        }
    }

    AssetHandle find_asset_by_name(const String& name)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED

        for (AssetPool& asset_pool : asset_registry->asset_pools)
        {
            for (u32 i = 0; i < asset_pool.data_pool.sparse_set.count; ++i)
            {
                AssetInfo* asset_info = &asset_pool.asset_infos[i];
                if (asset_info->name == name)
                {
                    return create_asset_handle(asset_info);
                }
            }
        }
        
        return {};
    }

    void get_assets_of_type(Type* type, Array<AssetHandle>& assets)
    {
        NIT_CHECK_ASSET_REGISTRY_CREATED
        AssetPool* pool = get_asset_pool_safe(type);
        for (u32 i = 0; i < pool->data_pool.sparse_set.count; ++i)
        {
            AssetInfo* info = &pool->asset_infos[i];
            assets.push_back(create_asset_handle(info));
        }
    }

    bool is_asset_valid(AssetHandle& asset)
    {
        AssetPool* pool = get_asset_pool(asset.type);
        if (!pool)
        {
            return false;
        }
        return pool::is_valid(&pool->data_pool, asset.data_id);
    }
    
    bool is_asset_loaded(AssetHandle& asset)
    {
        if (!is_asset_valid(asset))
        {
            return false;
        }
        
        return GetAssetInfoSafe(asset)->loaded;
    }

    AssetHandle create_asset(Type* type, const String& name, const String& path, void* data)
    {
        AssetPool* pool = get_asset_pool_safe(type);
        Pool* data_pool = &pool->data_pool;
        u32 data_id; pool::insert_data(data_pool, data_id, data);
        UUID asset_id = GenerateID();
        get_asset_registry_instance()->id_to_data_id.insert({asset_id, data_id});
        AssetInfo info{type, name, path, asset_id, get_last_asset_version(type), false, 0, data_id };
        push_asset_info(info,  pool::index_of(data_pool, data_id), true);
        AssetHandle asset_handle = create_asset_handle(&info);
        broadcast<const AssetCreatedArgs&>(get_asset_registry_instance()->asset_created_event, {asset_handle});
        return asset_handle;
    }

    void destroy_asset(AssetHandle& asset)
    {
        AssetPool* pool = GetAssetPoolSafe(asset);
        AssetInfo* info = GetAssetInfoSafe(asset);

        String remove_path = "assets/" + info->path;
        i32 res = std::remove(remove_path.c_str());
        NIT_CHECK(!res);
        
        free_asset(asset);

        AssetDestroyedArgs args;
        args.asset_handle = create_asset_handle(info);
        broadcast<const AssetDestroyedArgs&>(asset_registry->asset_destroyed_event, args);
        
        erase_asset_info(*info, pool::delete_data(&pool->data_pool, info->data_id));
        
        asset_registry->id_to_data_id.erase(asset.id);
    }

    void load_asset(AssetHandle& asset, bool force_reload)
    {
        AssetPool* pool = GetAssetPoolSafe(asset);
        AssetInfo* info = GetAssetInfoSafe(asset);
        
        if (info->loaded)
        {
            if (force_reload)
            {
                // In this case we should keep the reference count.
                u32 reference_count = info->reference_count;
                free_asset(asset);
                info->reference_count = reference_count;
            }
            else
            {
                return;
            }
        }
        
        info->loaded = true;
        load(asset.type, pool::get_raw_data(&pool->data_pool, asset.data_id));
    }

    void free_asset(AssetHandle& asset)
    {
        AssetPool* pool = GetAssetPoolSafe(asset);
        AssetInfo* info = GetAssetInfoSafe(asset);
        info->reference_count = 0;
        info->loaded = false;
        Free(asset.type, pool::get_raw_data(&pool->data_pool, asset.data_id));
    }

    void retain_asset(AssetHandle& asset)
    {
        if (!is_asset_valid(asset))
        {
            return;
        }

        AssetInfo* info = GetAssetInfoSafe(asset);

        if (!info->loaded)
        {
            load_asset(asset);
        }

        ++info->reference_count;
    }

    void release_asset(AssetHandle& asset, bool force_free)
    {
        if (!is_asset_valid(asset))
        {
            return;
        }
        
        AssetInfo* info = GetAssetInfoSafe(asset);

        if (!info->loaded)
        {
            return;
        }

        // A loaded asset with reference_count of 0 is a valid case.
        if (force_free || info->reference_count <= 1)
        {
            free_asset(asset);
            info->reference_count = 0;
            return;
        }
        
        --info->reference_count;
    }
}