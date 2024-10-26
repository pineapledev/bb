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

        pool->sparse_set.dense.reserve(max_element_count);
        pool->sparse_set.sparse.reserve(max_element_count);
        
        pool->type = GetType<T>();
        pool->elements  = new T[max_element_count];
        pool->max = max_element_count;
    }

    template<typename T>
    T& InsertDataWithID(MappedPool* pool, ID element_id, const T& data)
    {
        NIT_CHECK_MSG(pool->type == GetType<T>(), "Type mismatch!");
        NIT_CHECK_MSG(pool, "Invalid pool!");
        NIT_CHECK_MSG(pool->sparse_set.count < pool->max, "Max pool capacity reached!");
        return SetData(pool->elements, Insert(&pool->sparse_set, element_id), data);
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
        
        return GetDataPtr<T>(pool->elements, Search(&pool->sparse_set, element_id));
    }
    
    template<typename T>
    T& GetData(MappedPool* pool, ID element_id)
    {
        return *GetDataPtr<T>(pool->elements, element_id);
    }
}