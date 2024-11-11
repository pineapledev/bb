#pragma once

namespace nit
{
    template<typename T>
    void pool::load(Pool* pool, u32 max_element_count, bool self_id_management)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
            return;
        }
        
        if (!IsTypeRegistered<T>())
        {
            RegisterType<T>();
        }
        
        pool->type = GetType<T>();
        pool->elements  = new T[max_element_count];
        
        sparse::load(&pool->sparse_set, max_element_count);
        
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
    T* pool::insert_data_with_id(Pool* pool, u32 element_id, const T& data)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
            return nullptr;
        }

        if (sparse::IsFull(&pool->sparse_set))
        {
            pool::Resize(pool, pool->sparse_set.max * 2);
        }
        
        NIT_CHECK_MSG(pool->type == GetType<T>(), "Type mismatch!");
        return SetArrayData(pool->elements, sparse::Insert(&pool->sparse_set, element_id), data);
    }

    template<typename T>
    T* pool::InsertData(Pool* pool, u32& out_id, const T& data)
    {
        if (!pool || !pool->self_id_management)
        {
            NIT_DEBUGBREAK();
            return nullptr;
        }
        
        out_id = pool->available_ids.front();
        pool->available_ids.pop();
        return insert_data_with_id(pool, out_id, data);
    }
    
    template<typename T>
    T* pool::GetData(Pool* pool, u32 element_id)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
            return nullptr;
        }

        NIT_CHECK_MSG(pool->type == GetType<T>(), "Type mismatch!");

        if (!pool::is_valid(pool, element_id))
        {
            return nullptr;
        }
        
        u32 element_index = sparse::Search(&pool->sparse_set, element_id);

        if (element_index == SparseSet::INVALID)
        {
            return nullptr;
        }
        
        return GetArrayData<T>(pool->elements, element_index);
    }
}