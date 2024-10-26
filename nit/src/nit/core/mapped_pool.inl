#pragma once

namespace Nit
{
    template<typename T>
    void Load(MappedPool* pool, u32 max_element_count)
    {
        if (!IsTypeRegistered<T>())
        {
            RegisterType<T>();
        }

        pool->type = GetType<T>();
        pool->elements  = new T[max_element_count];
        pool->max = max_element_count;
    }

    template<typename T>
    T& InsertDataWithID(MappedPool* pool, ID element_id, const T& data)
    {
        NIT_CHECK_MSG(pool->type == GetType<T>(), "Type mismatch!");

        NIT_CHECK_MSG(pool, "Invalid pool!");
        
        u32 next_element = pool->count; 
        NIT_CHECK_MSG(next_element < pool->max, "Max pool capacity reached!");

        // "Create" the new element
        pool->element_id_to_index[element_id] = next_element;
        pool->index_to_element_id[next_element] = element_id;
        ++pool->count;

        // Insert the data in the next element slot (count)
        return SetData(pool->elements, next_element, data);
    }

    template<typename T>
    T& InsertData(MappedPool* pool, ID& out_id, const T& data)
    {
        out_id = GenerateID();
        return InsertDataWithID(pool, out_id, data);
    }
    
    template<typename T>
    T* GetDataPtr(MappedPool* pool, ID element_id)
    {
        NIT_CHECK_MSG(pool->type == GetType<T>(), "Type mismatch!");
        // Sanity checks
        NIT_CHECK_MSG(pool, "Invalid pool!");
        NIT_CHECK_MSG(element_id != 0, "Invalid pool id!");
        NIT_CHECK_MSG(pool->element_id_to_index.count(element_id) != 0, "Trying to get non-existent element!");
        
        // Retrieve the element data
        u32 element_index = pool->element_id_to_index.at(element_id);
        return GetDataPtr<T>(pool->elements, element_index);
    }
    
    template<typename T>
    T& GetData(MappedPool* pool, ID element_id)
    {
        NIT_CHECK_MSG(pool->type == GetType<T>(), "Type mismatch!");
        // Sanity checks
        NIT_CHECK_MSG(pool, "Invalid pool!");
        NIT_CHECK_MSG(element_id != 0, "Invalid pool id!");
        NIT_CHECK_MSG(pool->element_id_to_index.count(element_id) != 0, "Trying to get non-existent element!");
        
        // Retrieve the element data
        u32 element_index = pool->element_id_to_index.at(element_id);
        return GetData<T>(pool->elements, element_index);
    }
}