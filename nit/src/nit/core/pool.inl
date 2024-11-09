#pragma once

namespace Nit
{
    template<typename T>
    void FnPool::Load(Pool* pool, u32 max_element_count, bool self_id_management)
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
        
        FnSparseSet::Load(&pool->sparse_set, max_element_count);
        
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
    T* FnPool::InsertDataWithID(Pool* pool, u32 element_id, const T& data)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
            return nullptr;
        }

        if (FnSparseSet::IsFull(&pool->sparse_set))
        {
            FnPool::Resize(pool, pool->sparse_set.max * 2);
        }
        
        NIT_CHECK_MSG(pool->type == GetType<T>(), "Type mismatch!");
        return SetArrayData(pool->elements, FnSparseSet::Insert(&pool->sparse_set, element_id), data);
    }

    template<typename T>
    T* FnPool::InsertData(Pool* pool, u32& out_id, const T& data)
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
    T* FnPool::GetData(Pool* pool, u32 element_id)
    {
        if (!pool)
        {
            NIT_DEBUGBREAK();
            return nullptr;
        }

        NIT_CHECK_MSG(pool->type == GetType<T>(), "Type mismatch!");

        if (!FnPool::IsValid(pool, element_id))
        {
            return nullptr;
        }
        
        u32 element_index = FnSparseSet::Search(&pool->sparse_set, element_id);

        if (element_index == SparseSet::INVALID)
        {
            return nullptr;
        }
        
        return GetArrayData<T>(pool->elements, element_index);
    }
}