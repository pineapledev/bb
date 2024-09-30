#include "pool.h"

namespace Nit
{
    void FinishPool(Pool* pool)
    {
        NIT_CHECK_MSG(pool, "Invalid pool!");
        pool->type_name = "Invalid";
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
        NIT_CHECK_MSG(pool->set_data, "Set data function not found!");
        pool->set_data(pool->elements, next_element, data);
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
        NIT_CHECK_MSG(pool->get_data, "Get data function not found!");
        void* last_element_data = pool->get_data(pool->elements, last_element_index);
        NIT_CHECK_MSG(pool->set_data, "Set data function not found!");
        pool->set_data(pool->elements, deleted_element_index, last_element_data);
        
        // Updated the index associated with the last element id
        ID last_element_id = pool->index_to_element_id[last_element_index];
        pool->index_to_element_id.erase(last_element_index);
        pool->index_to_element_id[deleted_element_index] = last_element_id;
        pool->element_id_to_index[last_element_id] = deleted_element_index;
    }

    bool IsPoolElementValid(const Pool* pool, ID element_id)
    {
        NIT_CHECK_MSG(pool, "Invalid pool!");
        NIT_CHECK_MSG(element_id != 0, "Invalid pool id!");
        return pool->element_id_to_index.count(element_id) != 0;
    }

    void* GetPoolElementRawPtr(const Pool* pool, ID element_id)
    {
        // Sanity checks
        NIT_CHECK_MSG(pool, "Invalid pool!");
        NIT_CHECK_MSG(element_id != 0, "Invalid pool id!");
        NIT_CHECK_MSG(pool->element_id_to_index.count(element_id) != 0, "Trying to get non-existent element!");

        // Retrieve the element data
        u32 element_index = pool->element_id_to_index.at(element_id);
        NIT_CHECK_MSG(pool->get_data, "Get data function not found!");
        return pool->get_data(pool->elements, element_index);
    }

    void FinishPool(FastPool* pool)
    {
        NIT_CHECK_MSG(pool, "Invalid pool!");
        pool->type_name = "Invalid";
        free(pool->elements);
        pool->elements = nullptr;
        delete[] pool->index_to_element_id;
        pool->index_to_element_id = nullptr;
        delete[] pool->element_id_to_index;
        pool->element_id_to_index = nullptr;
        pool->count = 0;
        pool->max = 0;
    }

    void InsertPoolElementRawWithID(FastPool* pool, u32 element_id, void* data)
    {
        NIT_CHECK_MSG(pool, "Invalid pool!");

        u32 next_element = pool->count; 
        NIT_CHECK_MSG(next_element < pool->max, "Max pool capacity reached!");

        // "Create" the new element
        pool->element_id_to_index[element_id] = next_element;
        pool->index_to_element_id[next_element] = element_id;
        ++pool->count;
        
        // Insert the data in the next element slot (count)
        NIT_CHECK_MSG(pool->set_data, "Set data function not found!");
        pool->set_data(pool->elements, next_element, data);
    }

    void RemovePoolElement(FastPool* pool, u32 element_id)
    {
        if (pool->self_id_management)
        {
            pool->available_ids.push(element_id);
        }

        NIT_CHECK_MSG(pool, "Invalid pool!");
        
        // Retrieve the relevant indices
        NIT_CHECK_MSG(pool->element_id_to_index[element_id] != FastPool::INVALID_INDEX, "Trying to delete non-existent element!");
        u32 deleted_element_index = pool->element_id_to_index[element_id];
        u32 last_element_index = pool->count - 1;

        // "Delete" the element
        pool->element_id_to_index[element_id] = FastPool::INVALID_INDEX;
        --pool->count;

        // Put the last element data in the deleted element slot
        NIT_CHECK_MSG(pool->get_data, "Get data function not found!");
        void* last_element_data = pool->get_data(pool->elements, last_element_index);
        NIT_CHECK_MSG(pool->set_data, "Set data function not found!");
        pool->set_data(pool->elements, deleted_element_index, last_element_data);
        
        // Updated the index associated with the last element u32
        u32 last_element_id = pool->index_to_element_id[last_element_index];
        pool->index_to_element_id[deleted_element_index] = last_element_id;
    }

    bool IsPoolElementValid(const FastPool* pool, u32 element_id)
    {
        NIT_CHECK_MSG(pool, "Invalid pool!");
        return pool->element_id_to_index[element_id] != FastPool::INVALID_INDEX;
    }

    void* GetPoolElementRawPtr(const FastPool* pool, u32 element_id)
    {
        NIT_CHECK_MSG(pool, "Invalid pool!");
        NIT_CHECK_MSG(IsPoolElementValid(pool, element_id), "Trying to get non-existent element!");

        // Retrieve the element data
        u32 element_index = pool->element_id_to_index[element_id];
        NIT_CHECK_MSG(pool->get_data, "Get data function not found!");
        return pool->get_data(pool->elements, element_index);
    }
}