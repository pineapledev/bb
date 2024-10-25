#pragma once
#include "sparse_set.h"

namespace Nit
{
    constexpr u32 DEFAULT_POOL_ELEMENT_COUNT = 100;

    // Slower access to elements, less memory usage, serialization ready.
    struct Pool
    {
        Type*          type                = nullptr;
        void*          elements            = nullptr;
        Map<ID,  u32>  element_id_to_index = {};
        Map<u32, ID>   index_to_element_id = {};
        u32            count               = 0;
        u32            max                 = 0;
    };

    template<typename T>
    void InitPool(Pool* pool, u32 max_element_count = DEFAULT_POOL_ELEMENT_COUNT)
    {
        if (!IsTypeRegistered<T>())
        {
            RegisterType<T>();
        }

        pool->type = GetType<T>();
        pool->elements  = new T[max_element_count];
        pool->max = max_element_count;
    }
    
    void FinishPool(Pool* pool);
    
    void InsertPoolElementRawWithID(Pool* pool, ID element_id, void* data = nullptr);

    template<typename T>
    T& InsertPoolElementWithID(Pool* pool, ID element_id, const T& data)
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
    T& InsertPoolElement(Pool* pool, ID& out_id, const T& data)
    {
        out_id = GenerateID();
        return InsertPoolElementWithID(pool, out_id, data);
    }
    
    void RemovePoolElement(Pool* pool, ID element_id);

    bool IsPoolElementValid(Pool* pool, ID element_id);

    void* GetPoolElementRawPtr(Pool* pool, ID element_id);

    template<typename T>
    T* GetPoolElementPtr(Pool* pool, ID element_id)
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
    T& GetPoolElement(Pool* pool, ID element_id)
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

    // Quicker access to elements, more memory usage, can not be used in serialization coz the incremental ids.
    struct FastPool
    {
        static constexpr u32 INVALID_INDEX = U32_MAX;

        Type*          type                = nullptr;
        void*          elements            = nullptr;
        SparseSet      sparse_set          = {};
        Queue<u32>     available_ids       = {};
        bool           self_id_management  = false;
    };

    template<typename T>
    void InitPool(FastPool* pool, u32 max_element_count = DEFAULT_POOL_ELEMENT_COUNT, bool self_id_management = true)
    {
        if (!IsTypeRegistered<T>())
        {
            RegisterType<T>();
        }
        
        pool->type = GetType<T>();
        pool->elements  = new T[max_element_count];
        
        Load(&pool->sparse_set, DEFAULT_POOL_ELEMENT_COUNT);
        
        if (self_id_management)
        {
            for (u32 i = 0; i < max_element_count; ++i)
            {
                pool->available_ids.push(i);
            }
        }
        
        pool->self_id_management = self_id_management;
    }

    void FinishPool(FastPool* pool);
    
    bool  IsPoolElementValid(FastPool* pool, u32 element_id);
    
    void  InsertPoolElementRawWithID(FastPool* pool, u32 element_id, void* data);
    void  RemovePoolElement(FastPool* pool, u32 element_id);
    void* GetPoolElementRawPtr(FastPool* pool, u32 element_id);
    
    template<typename T>
    T& InsertPoolElementWithID(FastPool* pool, u32 element_id, const T& data)
    {
        NIT_CHECK_MSG(pool->type == GetType<T>(), "Type mismatch!");
        NIT_CHECK_MSG(pool, "Invalid pool!");
        return SetData(pool->elements, Insert(&pool->sparse_set, element_id), data);
    }

    template<typename T>
    T& InsertPoolElement(FastPool* pool, u32& out_id, const T& data = {})
    {
        NIT_CHECK_MSG(pool->self_id_management, "This pool does not manage own u32s!");
        out_id = pool->available_ids.front();
        pool->available_ids.pop();
        return InsertPoolElementWithID(pool, out_id, data);
    }
    
    template<typename T>
    T* GetPoolElementPtr(FastPool* pool, u32 element_id)
    {
        NIT_CHECK_MSG(pool->type == GetType<T>(), "Type mismatch!");
        NIT_CHECK_MSG(pool, "Invalid pool!");
        NIT_CHECK_MSG(IsPoolElementValid(pool, element_id), "Trying to get non-existent element!");
        
        u32 element_index = Search(&pool->sparse_set, element_id);

        if (element_index == SparseSet::INVALID_INDEX)
        {
            return nullptr;
        }
        
        return GetDataPtr<T>(pool->elements, element_index);
    }

    template<typename T>
    T& GetPoolElement(FastPool* pool, u32 element_id)
    {
        return *GetPoolElementPtr<T>(pool, element_id);
    }
}