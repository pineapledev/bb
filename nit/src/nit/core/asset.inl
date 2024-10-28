#pragma once

namespace Nit
{
    template<typename T>
    void RegisterAssetType(const AssetTypeArgs<T>& args, u32 version)
    {
        AssetRegistry* asset_registry = GetAssetRegistryInstance();
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
        SetDrawEditorFunction  (type,  args.fn_draw_editor);
        
        Load<T>(&asset_pool->data_pool, args.max_elements);
        asset_pool->asset_infos = new AssetInfo[args.max_elements];
    }
    
    template<typename T>
    AssetPool* GetAssetPool()
    {
        return GetAssetPool(GetType<T>());
    }
    
    template<typename T>
    bool IsAssetTypeRegistered()
    {
        return IsAssetTypeRegistered(GetType<T>());
    }
    
    template<typename T>
    u32 GetLastAssetVersion()
    {
        return GetLastAssetVersion(GetType<T>());
    }
    
    template<typename T>
    T* GetAssetData(const AssetHandle& asset)
    {
        AssetPool* pool = GetAssetPoolSafe(GetType<T>());
        return GetData<T>(&pool->data_pool, asset.id);
    }
    
    template<typename T>
    T* GetAssetDataPtr(const AssetHandle& asset)
    {
        AssetPool* pool = GetAssetPoolSafe(GetType<T>());
        return GetDataPtr<T>(&pool->data_pool, asset.id);
    }
    
    template<typename T>
    AssetHandle CreateAsset(const String& name, const String& path, const T& data)
    {
        AssetPool* pool = GetAssetPoolSafe(GetType<T>());
        PoolMap* data_pool = &pool->data_pool;
        Type* type = data_pool->type;
        ID id; InsertData(data_pool, id, data);
        AssetInfo info{type, name, path, id, GetLastAssetVersion<T>() };
        PushAssetInfo(info, IndexOf(data_pool, id), true);
        AssetHandle asset_handle{ name, type, id };
        Broadcast<const AssetCreatedArgs&>(GetAssetRegistryInstance()->asset_created_event, {asset_handle});
        return asset_handle;
    }
}

template<>
struct YAML::convert<Nit::AssetHandle>
{
    static Node encode(const Nit::AssetHandle& h)
    {
        Node node;
        node.push_back(h.name);
        node.push_back(h.type->name);
        node.push_back(h.id);
        node.SetStyle(EmitterStyle::Flow);
        return node;
    }
    
    static bool decode(const Node& node, Nit::AssetHandle& h)
    {
        if (!node.IsSequence() || node.size() != 3)
            return false;
        
        h.name = node[0].as<Nit::String>();
        h.type = Nit::GetType(node[1].as<Nit::String>());
        h.id   = node[2].as<Nit::ID>();
        return true;
    }
};

inline YAML::Emitter& operator<<(YAML::Emitter& out, const Nit::AssetHandle& h)
{
    out << YAML::Flow;
    out << YAML::BeginSeq << h.name << (h.type ? h.type->name : "") << h.id << YAML::EndSeq;
    return out;
}