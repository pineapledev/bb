#pragma once

namespace nit
{
    struct Pool
    {
        Type*          type                = nullptr;
        void*          elements            = nullptr;
        SparseSet      sparse_set          = {};
        Queue<u32>     available_ids       = {};
        bool           self_id_management  = false;
    };

    namespace pool
    {
        template<typename T>
        void Load(Pool* pool, u32 max_element_count, bool self_id_management = true);

        void Free(Pool* pool);

        bool IsValid(Pool* pool, u32 element_id);  
    
        bool InsertDataWithID(Pool* pool, u32 element_id, void* data = nullptr);

        bool InsertData(Pool* pool, u32& element_id, void* data = nullptr);
    
        template<typename T>
        T* InsertDataWithID(Pool* pool, u32 element_id, const T& data);
    
        template<typename T>
        T* InsertData(Pool* pool, u32& out_id, const T& data = {});

        u32 IndexOf(Pool* pool, u32 element_id);
    
        void* GetRawData(Pool* pool, u32 element_id);
    
        template<typename T>
        T* GetData(Pool* pool, u32 element_id);

        SparseSetDeletion DeleteData(Pool* pool, u32 element_id);
    
        void Resize(Pool* pool, u32 new_max);
    }
}

#include "nit/core/pool.inl"