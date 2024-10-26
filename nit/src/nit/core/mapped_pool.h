#pragma once

namespace Nit
{
    // Slower access to elements, less memory usage, serialization ready.
    struct MappedPool
    {
        static constexpr u32 DEFAULT_MAX = 100;
        
        Type*          type                = nullptr;
        void*          elements            = nullptr;
        Map<ID,  u32>  element_id_to_index = {};
        Map<u32, ID>   index_to_element_id = {};
        u32            count               = 0;
        u32            max                 = 0;
    };

    template<typename T>
    void Load(MappedPool* pool, u32 max_element_count = MappedPool::DEFAULT_MAX);

    void Free(MappedPool* pool);

    bool IsValid(MappedPool* pool, ID element_id);

    void InsertDataWithID(MappedPool* pool, ID element_id, void* data = nullptr);
    
    template<typename T>
    T& InsertDataWithID(MappedPool* pool, ID element_id, const T& data = {});

    template<typename T>
    T& InsertData(MappedPool* pool, ID& out_id, const T& data = {});

    void* GetDataRaw(MappedPool* pool, ID element_id);
    
    template<typename T>
    T* GetDataPtr(MappedPool* pool, ID element_id);

    template<typename T>
    T& GetData(MappedPool* pool, ID element_id);

    void DeleteData(MappedPool* pool, ID element_id);
}

#include "nit/core/mapped_pool.inl"