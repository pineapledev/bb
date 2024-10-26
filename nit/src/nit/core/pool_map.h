#pragma once

namespace Nit
{
    // Slower access to elements, less memory usage, serialization ready.
    struct PoolMap
    {
        static constexpr u32 DEFAULT_MAX = 100;
        
        Type*          type                = nullptr;
        void*          elements            = nullptr;
        SparseSetMap   sparse_set;
        u32            max                 = 0;
    };
    
    template<typename T>
    void Load(PoolMap* pool, u32 max_element_count = PoolMap::DEFAULT_MAX);
    
    void Free(PoolMap* pool);

    bool IsValid(PoolMap* pool, ID element_id);

    void InsertDataWithID(PoolMap* pool, ID element_id, void* data = nullptr);
    
    template<typename T>
    T& InsertDataWithID(PoolMap* pool, ID element_id, const T& data = {});

    template<typename T>
    T& InsertData(PoolMap* pool, ID& out_id, const T& data = {});

    u32 IndexOf(PoolMap* pool, ID element_id);
    
    void* GetDataRaw(PoolMap* pool, ID element_id);
    
    template<typename T>
    T* GetDataPtr(PoolMap* pool, ID element_id);

    template<typename T>
    T& GetData(PoolMap* pool, ID element_id);
    
    SparseSetDeletion DeleteData(PoolMap* pool, ID element_id);
}

#include "nit/core/pool_map.inl"
