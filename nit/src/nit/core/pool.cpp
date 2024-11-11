#include "pool.h"

namespace nit
{
    void pool::release(Pool* pool)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
            return;
        }
        
        pool->type = nullptr;
        free(pool->elements);
        pool->elements = nullptr;
        sparse::Free(&pool->sparse_set);
    }

    bool pool::is_valid(Pool* pool, u32 element_id)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
            return false;
        }
        
        return sparse::Test(&pool->sparse_set, element_id);
    }

    bool pool::insert_data_with_id(Pool* pool, u32 element_id, void* data)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
            return false;
        }

        if (sparse::IsFull(&pool->sparse_set))
        {
            pool::resize(pool, pool->sparse_set.max * 2);
        }
        
        SetArrayRawData(pool->type, pool->elements, sparse::Insert(&pool->sparse_set, element_id), data);
        return true;
    }

    bool pool::insert_data(Pool* pool, u32& element_id, void* data)
    {
        if (!pool || !pool->self_id_management)
        {
            NIT_DEBUGBREAK();
            return false;
        }
        
        element_id = pool->available_ids.front();
        pool->available_ids.pop();
        return pool::insert_data_with_id(pool, element_id, data);
    }

    SparseSetDeletion pool::delete_data(Pool* pool, u32 element_id)
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
        
        SparseSetDeletion deletion = sparse::Delete(&pool->sparse_set, element_id);
        void* last_element_data = GetArrayRawData(pool->type, pool->elements, deletion.last_slot);
        SetArrayRawData(pool->type, pool->elements, deletion.deleted_slot, last_element_data);
        return deletion;
    }

    void pool::resize(Pool* pool, u32 new_max)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
        }

        ResizeArray(pool->type, pool->elements, pool->sparse_set.max, new_max);
        sparse::Resize(&pool->sparse_set, new_max);
    }

    u32 pool::index_of(Pool* pool, u32 element_id)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
            return U32_MAX;
        }
        
        return sparse::Search(&pool->sparse_set, element_id);
    }

    void* pool::get_raw_data(Pool* pool, u32 element_id)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
            return nullptr;
        }
        
        u32 index = sparse::Search(&pool->sparse_set, element_id);
        return index != SparseSet::INVALID ? GetArrayRawData(pool->type, pool->elements, index) : nullptr;
    }
}