#include "pool.h"

namespace Nit
{
    void FnPool::Free(Pool* pool)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
            return;
        }
        
        pool->type = nullptr;
        free(pool->elements);
        pool->elements = nullptr;
        FnSparseSet::Free(&pool->sparse_set);
    }

    bool FnPool::IsValid(Pool* pool, u32 element_id)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
            return false;
        }
        
        return FnSparseSet::Test(&pool->sparse_set, element_id);
    }

    bool FnPool::InsertDataWithID(Pool* pool, u32 element_id, void* data)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
            return false;
        }

        if (FnSparseSet::IsFull(&pool->sparse_set))
        {
            FnPool::Resize(pool, pool->sparse_set.max * 2);
        }
        
        SetArrayRawData(pool->type, pool->elements, FnSparseSet::Insert(&pool->sparse_set, element_id), data);
        return true;
    }

    bool FnPool::InsertData(Pool* pool, u32& element_id, void* data)
    {
        if (!pool || !pool->self_id_management)
        {
            NIT_DEBUGBREAK();
            return false;
        }
        
        element_id = pool->available_ids.front();
        pool->available_ids.pop();
        return FnPool::InsertDataWithID(pool, element_id, data);
    }

    SparseSetDeletion FnPool::DeleteData(Pool* pool, u32 element_id)
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
        
        SparseSetDeletion deletion = FnSparseSet::Delete(&pool->sparse_set, element_id);
        void* last_element_data = GetArrayRawData(pool->type, pool->elements, deletion.last_slot);
        SetArrayRawData(pool->type, pool->elements, deletion.deleted_slot, last_element_data);
        return deletion;
    }

    void FnPool::Resize(Pool* pool, u32 new_max)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
        }

        ResizeArray(pool->type, pool->elements, pool->sparse_set.max, new_max);
        FnSparseSet::Resize(&pool->sparse_set, new_max);
    }

    u32 FnPool::IndexOf(Pool* pool, u32 element_id)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
            return U32_MAX;
        }
        
        return FnSparseSet::Search(&pool->sparse_set, element_id);
    }

    void* FnPool::GetRawData(Pool* pool, u32 element_id)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
            return nullptr;
        }
        
        u32 index = FnSparseSet::Search(&pool->sparse_set, element_id);
        return index != SparseSet::INVALID ? GetArrayRawData(pool->type, pool->elements, index) : nullptr;
    }
}