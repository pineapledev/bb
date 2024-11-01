﻿#pragma once

namespace Nit
{
    template<typename T>
    void Load(Pool* pool, u32 max_element_count, bool self_id_management)
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
    T* InsertDataWithID(Pool* pool, u32 element_id, const T& data)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
            return nullptr;
        }

        if (IsFull(&pool->sparse_set))
        {
            Resize(pool, pool->sparse_set.max * 2);
        }
        
        NIT_CHECK_MSG(pool->type == GetType<T>(), "Type mismatch!");
        return SetData(pool->elements, Insert(&pool->sparse_set, element_id), data);
    }

    template<typename T>
    T* InsertData(Pool* pool, u32& out_id, const T& data)
    {
        if (!pool || !pool->self_id_management)
        {
            NIT_DEBUGBREAK();
            return nullptr;
        }
        
        out_id = pool->available_ids.front();
        pool->available_ids.pop();
        return InsertDataWithID(pool, out_id, data);
    }
    
    template<typename T>
    T* GetData(Pool* pool, u32 element_id)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
            return nullptr;
        }

        NIT_CHECK_MSG(pool->type == GetType<T>(), "Type mismatch!");

        if (!IsValid(pool, element_id))
        {
            return nullptr;
        }
        
        u32 element_index = Search(&pool->sparse_set, element_id);

        if (element_index == SparseSet::INVALID)
        {
            return nullptr;
        }
        
        return GetData<T>(pool->elements, element_index);
    }
}