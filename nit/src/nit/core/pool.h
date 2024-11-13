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

    void              pool_free(Pool* pool);
    bool              pool_is_valid(Pool* pool, u32 element_id);  
    bool              pool_insert_data_with_id(Pool* pool, u32 element_id, void* data = nullptr);
    bool              pool_insert_data(Pool* pool, u32& element_id, void* data = nullptr);
    u32               pool_index_of(Pool* pool, u32 element_id);
    void*             pool_get_raw_data(Pool* pool, u32 element_id);
    SparseSetDeletion pool_delete_data(Pool* pool, u32 element_id);
    void              pool_resize(Pool* pool, u32 new_max);
    
    template<typename T> void pool_load(Pool* pool, u32 max_element_count, bool self_id_management = true);
    template<typename T> T*   pool_insert_data_with_id(Pool* pool, u32 element_id, const T& data);
    template<typename T> T*   pool_insert_data(Pool* pool, u32& out_id, const T& data = {});
    template<typename T> T*   pool_get_data(Pool* pool, u32 element_id);
}

#include "nit/core/pool.inl"