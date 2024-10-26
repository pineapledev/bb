#pragma once

namespace Nit
{
    template<typename T>
    void Load(Pool* pool, u32 max_element_count, bool self_id_management)
    {
        if (!IsTypeRegistered<T>())
        {
            RegisterType<T>();
        }
        
        pool->type = GetType<T>();
        pool->elements  = new T[max_element_count];
        
        Load(&pool->sparse_set, max_element_count);
        
        if (self_id_management)
        {
            for (u32 i = 0; i < max_element_count; ++i)
            {
                pool->available_ids.push(i);
            }
        }
        
        pool->self_id_management = self_id_management;
    }
    
    template<typename T>
    T& InsertDataWithID(Pool* pool, u32 element_id, const T& data)
    {
        NIT_CHECK_MSG(pool->type == GetType<T>(), "Type mismatch!");
        NIT_CHECK_MSG(pool, "Invalid pool!");
        return SetData(pool->elements, Insert(&pool->sparse_set, element_id), data);
    }

    template<typename T>
    T& InsertData(Pool* pool, u32& out_id, const T& data)
    {
        NIT_CHECK_MSG(pool->self_id_management, "This pool does not manage own u32s!");
        out_id = pool->available_ids.front();
        pool->available_ids.pop();
        return InsertDataWithID(pool, out_id, data);
    }
    
    template<typename T>
    T* GetDataPtr(Pool* pool, u32 element_id)
    {
        NIT_CHECK_MSG(pool->type == GetType<T>(), "Type mismatch!");
        NIT_CHECK_MSG(pool, "Invalid pool!");
        NIT_CHECK_MSG(IsValid(pool, element_id), "Trying to get non-existent element!");
        
        u32 element_index = Search(&pool->sparse_set, element_id);

        if (element_index == SparseSet::INVALID_INDEX)
        {
            return nullptr;
        }
        
        return GetDataPtr<T>(pool->elements, element_index);
    }
    
    template<typename T>
    T& GetData(Pool* pool, u32 element_id)
    {
        return *GetDataPtr<T>(pool, element_id);
    }
}