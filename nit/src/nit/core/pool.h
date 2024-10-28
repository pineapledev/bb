#pragma once

namespace Nit
{
    // Quicker access to elements, more memory usage, can not be used in serialization coz the incremental ids.
    struct Pool
    {
        static constexpr u32 DEFAULT_MAX = 100;
        
        Type*          type                = nullptr;
        void*          elements            = nullptr;
        SparseSet      sparse_set          = {};
        Queue<u32>     available_ids       = {};
        bool           self_id_management  = false;
    };

    template<typename T>
    void Load(Pool* pool, u32 max_element_count = Pool::DEFAULT_MAX, bool self_id_management = true);

    void Free(Pool* pool);

    bool IsValid(Pool* pool, u32 element_id);  
    
    void InsertDataWithID(Pool* pool, u32 element_id, void* data = nullptr);

    void InsertData(Pool* pool, u32& element_id, void* data = nullptr);
    
    template<typename T>
    T& InsertDataWithID(Pool* pool, u32 element_id, const T& data);
    
    template<typename T>
    T& InsertData(Pool* pool, u32& out_id, const T& data = {});

    u32 IndexOf(Pool* pool, u32 element_id);
    
    void* GetDataRaw(Pool* pool, u32 element_id);
    
    template<typename T>
    T* GetDataPtr(Pool* pool, u32 element_id);

    template<typename T>
    T& GetData(Pool* pool, u32 element_id);
    
    SparseSetDeletion DeleteData(Pool* pool, u32 element_id);
}

#include "nit/core/pool.inl"