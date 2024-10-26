#include "pool_map.h"

namespace Nit
{
    void Free(PoolMap* pool)
    {
        NIT_CHECK_MSG(pool, "Invalid pool!");
        pool->type = nullptr;
        free(pool->elements);
        pool->elements = nullptr;
        pool->sparse_set = {};
        pool->max = 0;
    }

    bool IsValid(PoolMap* pool, ID element_id)
    {
        NIT_CHECK_MSG(pool, "Invalid pool!");
        return Test(&pool->sparse_set, element_id);
    }
    
    void InsertDataWithID(PoolMap* pool, ID element_id, void* data)
    {
        NIT_CHECK_MSG(pool, "Invalid pool!");
        return SetRawData(pool->type, pool->elements, Insert(&pool->sparse_set, element_id), data);
    }

    void DeleteData(PoolMap* pool, ID element_id)
    {
        NIT_CHECK_MSG(pool, "Invalid pool!");
        SparseSetDeletion deletion = Delete(&pool->sparse_set, element_id);
        void* last_element_data = GetRawData(pool->type, pool->elements, deletion.last_slot);
        SetRawData(pool->type, pool->elements, deletion.deleted_slot, last_element_data);
    }

    void* GetDataRaw(PoolMap* pool, ID element_id)
    {
        NIT_CHECK_MSG(pool, "Invalid pool!");
        return GetRawData(pool->type, pool->elements, Search(&pool->sparse_set, element_id));
    }
}