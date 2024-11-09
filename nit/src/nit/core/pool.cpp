#include "pool.h"

namespace Nit
{
    void PoolProc::Free(Pool* pool)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
            return;
        }
        
        pool->type = nullptr;
        free(pool->elements);
        pool->elements = nullptr;
        SparseSetProc::Free(&pool->sparse_set);
    }

    bool PoolProc::IsValid(Pool* pool, u32 element_id)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
            return false;
        }
        
        return SparseSetProc::Test(&pool->sparse_set, element_id);
    }

    bool PoolProc::InsertDataWithID(Pool* pool, u32 element_id, void* data)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
            return false;
        }

        if (SparseSetProc::IsFull(&pool->sparse_set))
        {
            PoolProc::Resize(pool, pool->sparse_set.max * 2);
        }
        
        SetArrayRawData(pool->type, pool->elements, SparseSetProc::Insert(&pool->sparse_set, element_id), data);
        return true;
    }

    bool PoolProc::InsertData(Pool* pool, u32& element_id, void* data)
    {
        if (!pool || !pool->self_id_management)
        {
            NIT_DEBUGBREAK();
            return false;
        }
        
        element_id = pool->available_ids.front();
        pool->available_ids.pop();
        return PoolProc::InsertDataWithID(pool, element_id, data);
    }

    SparseSetDeletion PoolProc::DeleteData(Pool* pool, u32 element_id)
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
        
        SparseSetDeletion deletion = SparseSetProc::Delete(&pool->sparse_set, element_id);
        void* last_element_data = GetArrayRawData(pool->type, pool->elements, deletion.last_slot);
        SetArrayRawData(pool->type, pool->elements, deletion.deleted_slot, last_element_data);
        return deletion;
    }

    void PoolProc::Resize(Pool* pool, u32 new_max)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
        }

        ResizeArray(pool->type, pool->elements, pool->sparse_set.max, new_max);
        SparseSetProc::Resize(&pool->sparse_set, new_max);
    }

    u32 PoolProc::IndexOf(Pool* pool, u32 element_id)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
            return U32_MAX;
        }
        
        return SparseSetProc::Search(&pool->sparse_set, element_id);
    }

    void* PoolProc::GetRawData(Pool* pool, u32 element_id)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
            return nullptr;
        }
        
        u32 index = SparseSetProc::Search(&pool->sparse_set, element_id);
        return index != SparseSet::INVALID ? GetArrayRawData(pool->type, pool->elements, index) : nullptr;
    }
}