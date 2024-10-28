#include "pool.h"

namespace Nit
{
    void Free(Pool* pool)
    {
        NIT_CHECK_MSG(pool, "Invalid pool!");
        pool->type = nullptr;
        free(pool->elements);
        pool->elements = nullptr;
        Free(&pool->sparse_set);
    }

    bool IsValid(Pool* pool, u32 element_id)
    {
        NIT_CHECK_MSG(pool, "Invalid pool!");
        return Test(&pool->sparse_set, element_id);
    }

    void InsertDataWithID(Pool* pool, u32 element_id, void* data)
    {
        NIT_CHECK_MSG(pool, "Invalid pool!");
        return SetData(pool->type, pool->elements, Insert(&pool->sparse_set, element_id), data);
    }

    void InsertData(Pool* pool, u32& element_id, void* data)
    {
        NIT_CHECK_MSG(pool->self_id_management, "This pool does not manage own u32s!");
        element_id = pool->available_ids.front();
        pool->available_ids.pop();
        return InsertDataWithID(pool, element_id, data);
    }

    SparseSetDeletion DeleteData(Pool* pool, u32 element_id)
    {
        if (pool->self_id_management)
        {
            pool->available_ids.push(element_id);
        }
        
        NIT_CHECK_MSG(pool, "Invalid pool!");
        SparseSetDeletion deletion = Delete(&pool->sparse_set, element_id);
        void* last_element_data = GetData(pool->type, pool->elements, deletion.last_slot);
        SetData(pool->type, pool->elements, deletion.deleted_slot, last_element_data);
        return deletion;
    }

    u32 IndexOf(Pool* pool, u32 element_id)
    {
        return Search(&pool->sparse_set, element_id);
    }

    void* GetDataRaw(Pool* pool, u32 element_id)
    {
        NIT_CHECK_MSG(pool, "Invalid pool!");
        NIT_CHECK(element_id != SparseSet::INVALID_INDEX);
        return GetData(pool->type, pool->elements, Search(&pool->sparse_set, element_id));
    }
}