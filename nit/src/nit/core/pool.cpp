#include "pool.h"

namespace Nit
{
    void Free(Pool* pool)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
            return;
        }
        
        pool->type = nullptr;
        free(pool->elements);
        pool->elements = nullptr;
        Free(&pool->sparse_set);
    }

    bool IsValid(Pool* pool, u32 element_id)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
            return false;
        }
        
        return Test(&pool->sparse_set, element_id);
    }

    bool InsertDataWithID(Pool* pool, u32 element_id, void* data)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
            return false;
        }

        if (IsFull(&pool->sparse_set))
        {
            Resize(pool, pool->sparse_set.max * 2);
        }
        
        SetData(pool->type, pool->elements, Insert(&pool->sparse_set, element_id), data);
        return true;
    }

    bool InsertData(Pool* pool, u32& element_id, void* data)
    {
        if (!pool || !pool->self_id_management)
        {
            NIT_DEBUGBREAK();
            return false;
        }
        
        element_id = pool->available_ids.front();
        pool->available_ids.pop();
        return InsertDataWithID(pool, element_id, data);
    }

    SparseSetDeletion DeleteData(Pool* pool, u32 element_id)
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
        
        SparseSetDeletion deletion = Delete(&pool->sparse_set, element_id);
        void* last_element_data = GetData(pool->type, pool->elements, deletion.last_slot);
        SetData(pool->type, pool->elements, deletion.deleted_slot, last_element_data);
        return deletion;
    }

    void Resize(Pool* pool, u32 new_max)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
        }

        ResizeData(pool->type, pool->elements, pool->sparse_set.max, new_max);
        Resize(&pool->sparse_set, new_max);
    }

    u32 IndexOf(Pool* pool, u32 element_id)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
            return U32_MAX;
        }
        
        return Search(&pool->sparse_set, element_id);
    }

    void* GetDataRaw(Pool* pool, u32 element_id)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
            return nullptr;
        }
        
        u32 index = Search(&pool->sparse_set, element_id);
        return index != SparseSet::INVALID ? GetData(pool->type, pool->elements, index) : nullptr;
    }
}