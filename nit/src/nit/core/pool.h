#pragma once

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
        
        pool->elements  = new T[max_element_count];
        pool->max = max_element_count;
    }
    
    void FinishPool(Pool* pool);
    
    void InsertPoolElementRawWithID(Pool* pool, ID element_id, void* data = nullptr);

    template<typename T>
    void InsertPoolElementWithID(Pool* pool, ID element_id, T data)
    {
        NIT_CHECK_MSG(pool->type == GetType<T>(), "Type mismatch!");
        InsertPoolElementRawWithID(pool, element_id, &data);
    }

    template<typename T>
    void InsertPoolElement(Pool* pool, ID& out_id, const T& data)
    {
        out_id = GenerateID();
        InsertPoolElementWithID(pool, out_id, data);
    }
    
    void RemovePoolElement(Pool* pool, ID element_id);

    bool IsPoolElementValid(const Pool* pool, ID element_id);

    void* GetPoolElementRawPtr(const Pool* pool, ID element_id);

    template<typename T>
    T* GetPoolElementPtr(const Pool* pool, ID element_id)
    {
        NIT_CHECK_MSG(pool->type == GetType<T>(), "Type mismatch!");
        return static_cast<T*>(GetPoolElementRawPtr(pool, element_id));
    }

    template<typename T>
    T& GetPoolElement(const Pool* pool, ID element_id)
    {
        return *GetPoolElementPtr<T>(pool, element_id);
    }

    // Quicker access to elements, more memory usage, can not be used in serialization coz the incremental ids.
    struct FastPool
    {
        static constexpr u32 INVALID_INDEX = U32_MAX;

        Type*          type                = nullptr;
        void*          elements            = nullptr;
        u32*           element_id_to_index = nullptr;
        u32*           index_to_element_id = nullptr;
        u32            count               = 0;
        u32            max                 = 0;
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
        
        pool->element_id_to_index = new u32[max_element_count];
        pool->index_to_element_id = new u32[max_element_count];
        
        FillRaw(pool->element_id_to_index, max_element_count, FastPool::INVALID_INDEX);
        FillRaw(pool->index_to_element_id, max_element_count, 0);

        pool->max = max_element_count;
        
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
    
    void InsertPoolElementRawWithID(FastPool* pool, u32 element_id, void* data);

    template<typename T>
    void InsertPoolElementWithID(FastPool* pool, u32 element_id, T data)
    {
        NIT_CHECK_MSG(pool->type == GetType<T>(), "Type mismatch!");
        InsertPoolElementRawWithID(pool, element_id, &data);
    }

    template<typename T>
    void InsertPoolElement(FastPool* pool, u32& out_id, const T& data)
    {
        NIT_CHECK_MSG(pool->self_id_management, "This pool does not manage own u32s!");
        out_id = pool->available_ids.front();
        pool->available_ids.pop();
        InsertPoolElementWithID(pool, out_id, data);
    }
    
    void RemovePoolElement(FastPool* pool, u32 element_id);

    bool IsPoolElementValid(const FastPool* pool, u32 element_id);
    
    void* GetPoolElementRawPtr(const FastPool* pool, u32 element_id);

    template<typename T>
    T* GetPoolElementPtr(const FastPool* pool, u32 element_id)
    {
        NIT_CHECK_MSG(pool->type == GetType<T>(), "Type mismatch!");
        return static_cast<T*>(GetPoolElementRawPtr(pool, element_id));
    }

    template<typename T>
    T& GetPoolElement(const FastPool* pool, u32 element_id)
    {
        return *GetPoolElementPtr<T>(pool, element_id);
    }
}