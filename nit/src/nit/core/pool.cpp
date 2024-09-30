#include "pool.h"

namespace Nit
{
    ID InsertPoolElementRawWithID(Pool* pool, void* data, ID element_id)
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
        
        return element_id;
    }
    
    void RemovePoolElement(Pool* pool, ID element_id)
    {
        NIT_CHECK_MSG(pool, "Invalid pool!");
        
        // Retrieve the relevant indices
        NIT_CHECK_MSG(element_id != 0, "Invalid pool id!");
        NIT_CHECK_MSG(pool->element_id_to_index.count(element_id) != 0, "Trying to delete non-existent element!");
        u32 deleted_element_index = pool->element_id_to_index[element_id];
        u32 last_element_index = pool->count - 1;

        // "Delete" the pool
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
    
    void* GetPoolElementRawPtr(Pool* pool, ID element_id)
    {
        // Sanity checks
        NIT_CHECK_MSG(pool, "Invalid pool!");
        NIT_CHECK_MSG(element_id != 0, "Invalid pool id!");
        NIT_CHECK_MSG(pool->element_id_to_index.count(element_id) != 0, "Trying to get non-existent element!");

        // Retrieve the element data
        u32 element_index = pool->element_id_to_index[element_id];
        NIT_CHECK_MSG(pool->get_data, "Get data function not found!");
        return pool->get_data(pool->elements, element_index);
    }
}