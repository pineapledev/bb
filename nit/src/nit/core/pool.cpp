#include "pool.h"

namespace nit
{
    void pool_free(Pool* pool)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
            return;
        }
        
        pool->type = nullptr;
        free(pool->elements);
        pool->elements = nullptr;
        sparse_release(&pool->sparse_set);
    }

    bool pool_is_valid(Pool* pool, u32 element_id)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
            return false;
        }
        
        return sparse_test(&pool->sparse_set, element_id);
    }

    bool pool_insert_data_with_id(Pool* pool, u32 element_id, void* data)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
            return false;
        }

        if (sparse_is_full(&pool->sparse_set))
        {
            pool_resize(pool, pool->sparse_set.max * 2);
        }
        
        set_array_raw_data(pool->type, pool->elements, sparse_insert(&pool->sparse_set, element_id), data);
        return true;
    }

    bool pool_insert_data(Pool* pool, u32& element_id, void* data)
    {
        if (!pool || !pool->self_id_management)
        {
            NIT_DEBUGBREAK();
            return false;
        }
        
        element_id = pool->available_ids.front();
        pool->available_ids.pop();
        return pool_insert_data_with_id(pool, element_id, data);
    }

    SparseSetDeletion pool_delete_data(Pool* pool, u32 element_id)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
            return { false };
        }
        
        if (pool->self_id_management)
        {
            pool->available_ids.push(element_id);
        }
        
        SparseSetDeletion deletion = sparse_remove(&pool->sparse_set, element_id);
        void* last_element_data = get_array_raw_data(pool->type, pool->elements, deletion.last_slot);
        set_array_raw_data(pool->type, pool->elements, deletion.deleted_slot, last_element_data);
        return deletion;
    }

    void pool_resize(Pool* pool, u32 new_max)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
        }

        resize_array(pool->type, pool->elements, pool->sparse_set.max, new_max);
        sparse_resize(&pool->sparse_set, new_max);
    }

    u32 pool_index_of(Pool* pool, u32 element_id)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
            return U32_MAX;
        }
        
        return sparse_search(&pool->sparse_set, element_id);
    }

    void* pool_get_raw_data(Pool* pool, u32 element_id)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
            return nullptr;
        }
        
        u32 index = sparse_search(&pool->sparse_set, element_id);
        return index != SparseSet::INVALID ? get_array_raw_data(pool->type, pool->elements, index) : nullptr;
    }
}