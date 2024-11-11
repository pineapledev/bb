#pragma once

namespace nit
{
    template<typename T>
    void register_asset_type(const AssetTypeArgs<T>& args, u32 version)
    {
        AssetRegistry* asset_registry = get_asset_registry_instance();
        AssetPool* asset_pool = &asset_registry->asset_pools.emplace_back();
        
        if (!IsTypeRegistered<T>())
        {
            RegisterType<T>();
        }
        
        Type* type = GetType<T>();
        
        SetInvokeLoadFunction  (type,  args.fn_load);
        SetInvokeFreeFunction  (type,  args.fn_free);
        SetSerializeFunction   (type,  args.fn_serialize);
        SetDeserializeFunction (type,  args.fn_deserialize);
#ifdef NIT_EDITOR_ENABLED
        SetDrawEditorFunction  (type,  args.fn_draw_editor);
#endif
        pool::load<T>(&asset_pool->data_pool, args.max_elements);
        asset_pool->asset_infos = new AssetInfo[args.max_elements];
    }
    
    template<typename T>
    AssetPool* get_asset_pool()
    {
        return get_asset_pool(GetType<T>());
    }
    
    template<typename T>
    bool is_asset_type_registered()
    {
        return is_asset_type_registered(GetType<T>());
    }
    
    template<typename T>
    u32 get_last_asset_version()
    {
        return get_last_asset_version(GetType<T>());
    }
    
    template<typename T>
    T* get_asset_data(AssetHandle& asset)
    {
        AssetPool* pool = get_asset_pool_safe(GetType<T>());
        return pool::GetData<T>(&pool->data_pool, asset.data_id);
    }
    
    template<typename T>
    AssetHandle create_asset(const String& name, const String& path, const T& data)
    {
        AssetPool* pool = get_asset_pool_safe(GetType<T>());
        Pool* data_pool = &pool->data_pool;
        Type* type = data_pool->type;
        u32 data_id; pool::InsertData(data_pool, data_id, data);
        UUID asset_id = GenerateID();
        get_asset_registry_instance()->id_to_data_id.insert({asset_id, data_id});
        AssetInfo info{type, name, path, asset_id, get_last_asset_version<T>(), false, 0, data_id };
        push_asset_info(info, pool::IndexOf(data_pool, data_id), true);
        AssetHandle asset_handle = create_asset_handle(&info);
        broadcast<const AssetCreatedArgs&>(get_asset_registry_instance()->asset_created_event, {asset_handle});
        return asset_handle;
    }
}

template<>
struct YAML::convert<nit::AssetHandle>
{
    static Node encode(const nit::AssetHandle& h)
    {
        Node node;
        node.push_back(h.name);
        node.push_back(h.type->name);
        node.push_back((nit::u64) h.id);
        node.SetStyle(EmitterStyle::Flow);
        return node;
    }
    
    static bool decode(const Node& node, nit::AssetHandle& h)
    {
        if (!node.IsSequence() || node.size() != 3)
            return false;

        h.name    = node[0].as<nit::String>();
        h.type    = nit::GetType(node[1].as<nit::String>());
        h.id      = (nit::UUID) node[2].as<nit::u64>();
        return true;
    }
};

inline YAML::Emitter& operator<<(YAML::Emitter& out, const nit::AssetHandle& h)
{
    out << YAML::Flow;
    out << YAML::BeginSeq << h.name << (h.type ? h.type->name : "") << (nit::u64) h.id << YAML::EndSeq;
    return out;
}