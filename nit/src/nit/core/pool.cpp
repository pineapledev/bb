
namespace Nit
{
    void FinishPool(Pool* pool)
    {
        NIT_CHECK_MSG(pool, "Invalid pool!");
        pool->type = nullptr;
        free(pool->elements);
        pool->elements = nullptr;
        pool->index_to_element_id.clear();
        pool->element_id_to_index.clear();
        pool->count = 0;
        pool->max = 0;
    }

    void InsertPoolElementRawWithID(Pool* pool, ID element_id, void* data)
    {
        NIT_CHECK_MSG(pool, "Invalid pool!");

        u32 next_element = pool->count;
        NIT_CHECK_MSG(next_element < pool->max, "Max pool capacity reached!");

        // "Create" the new element
        pool->element_id_to_index[element_id] = next_element;
        pool->index_to_element_id[next_element] = element_id;
        ++pool->count;

        // Insert the data in the next element slot (count)
        SetRawData(pool->type, pool->elements, next_element, data);
    }

    void RemovePoolElement(Pool* pool, ID element_id)
    {
        NIT_CHECK_MSG(pool, "Invalid pool!");

        // Retrieve the relevant indices
        NIT_CHECK_MSG(element_id != 0, "Invalid pool id!");
        NIT_CHECK_MSG(pool->element_id_to_index.count(element_id) != 0, "Trying to delete non-existent element!");
        u32 deleted_element_index = pool->element_id_to_index[element_id];
        u32 last_element_index = pool->count - 1;

        // "Delete" the element
        pool->element_id_to_index.erase(element_id);
        --pool->count;

        // Put the last element data in the deleted element slot
        void* last_element_data = GetRawData(pool->type, pool->elements, last_element_index);
        SetRawData(pool->type, pool->elements, deleted_element_index, last_element_data);

        // Updated the index associated with the last element id
        ID last_element_id = pool->index_to_element_id[last_element_index];
        pool->index_to_element_id.erase(last_element_index);
        pool->index_to_element_id[deleted_element_index] = last_element_id;
        pool->element_id_to_index[last_element_id] = deleted_element_index;
    }

    bool IsPoolElementValid(Pool* pool, ID element_id)
    {
        NIT_CHECK_MSG(pool, "Invalid pool!");
        NIT_CHECK_MSG(element_id != 0, "Invalid pool id!");
        return pool->element_id_to_index.count(element_id) != 0;
    }

    void* GetPoolElementRawPtr(Pool* pool, ID element_id)
    {
        // Sanity checks
        NIT_CHECK_MSG(pool, "Invalid pool!");
        NIT_CHECK_MSG(element_id != 0, "Invalid pool id!");
        NIT_CHECK_MSG(pool->element_id_to_index.count(element_id) != 0, "Trying to get non-existent element!");

        // Retrieve the element data
        u32 element_index = pool->element_id_to_index.at(element_id);
        return GetRawData(pool->type, pool->elements, element_index);
    }

    void FinishPool(FastPool* pool)
    {
        NIT_CHECK_MSG(pool, "Invalid pool!");
        pool->type = nullptr;
        free(pool->elements);
        pool->elements = nullptr;
        Free(&pool->sparse_set);
    }

    bool IsPoolElementValid(FastPool* pool, u32 element_id)
    {
        NIT_CHECK_MSG(pool, "Invalid pool!");
        return Test(&pool->sparse_set, element_id);
        return true;
    }

    void InsertPoolElementRawWithID(FastPool* pool, u32 element_id, void* data)
    {
        NIT_CHECK_MSG(pool, "Invalid pool!");
        return SetRawData(pool->type, pool->elements, Insert(&pool->sparse_set, element_id), data);
    }

    void RemovePoolElement(FastPool* pool, u32 element_id)
    {
        if (pool->self_id_management)
        {
            pool->available_ids.push(element_id);
        }

        NIT_CHECK_MSG(pool, "Invalid pool!");
        void* last_element_data = GetRawData(pool->type, pool->elements, Search(&pool->sparse_set, pool->sparse_set.count));
        SetRawData(pool->type, pool->elements, Search(&pool->sparse_set, element_id), last_element_data);
        Delete(&pool->sparse_set, element_id);
    }

    void* GetPoolElementRawPtr(FastPool* pool, u32 element_id)
    {
        NIT_CHECK_MSG(pool, "Invalid pool!");
        return GetRawData(pool->type, pool->elements, Search(&pool->sparse_set, element_id));
    }
}