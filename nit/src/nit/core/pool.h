#pragma once

namespace Nit
{
    constexpr u32 DEFAULT_POOL_ELEMENT_COUNT = 100;

    // Slower access to elements, less memory usage, serialization ready.
    struct Pool
    {
        using SetDataFunc = void  (*) (void*, u32, void*);
        using GetDataFunc = void* (*) (void*, u32);

        const char*    type_name           = "Invalid";
        void*          elements            = nullptr;
        SetDataFunc    set_data            = nullptr;
        GetDataFunc    get_data            = nullptr;
        Map<ID,  u32>  element_id_to_index = {};
        Map<u32, ID>   index_to_element_id = {};
        u32            count               = 0;
        u32            max                 = 0;
    };

    template<typename T>
    void InitPool(Pool* pool, u32 max_element_count = DEFAULT_POOL_ELEMENT_COUNT)
    {
        pool->type_name = typeid(T).name();
        pool->elements = new T[max_element_count];
        
        pool->set_data = [](void* elements, u32 index, void* data) {
            NIT_CHECK_MSG(elements, "Invalid element array!");
            NIT_CHECK_MSG(data, "Invalid data!");
            T* casted_data     = static_cast<T*>(data);
            T* casted_elements = static_cast<T*>(elements);
            casted_elements[index] = *casted_data;
        };
        
        pool->get_data = [](void* elements, u32 index) -> void* {
            NIT_CHECK_MSG(elements, "Invalid element array!");
            T* casted_elements = static_cast<T*>(elements);
            T* data = &casted_elements[index];
            NIT_CHECK_MSG(data, "Invalid data!");
            return data;
        };

        pool->max = max_element_count;
    }
    
    template<typename T>
    void FinishPool(Pool* pool)
    {
        NIT_CHECK_MSG(pool, "Invalid pool!");
        pool->type_name = "Invalid";
        T* casted_elements = static_cast<T*>(pool->elements);
        delete[] casted_elements;
        pool->elements = nullptr;
        pool->index_to_element_id.clear();
        pool->element_id_to_index.clear();
        pool->count = 0;
        pool->max = 0;
    }
    
    void InsertPoolElementRawWithID(Pool* pool, ID element_id, void* data);

    template<typename T>
    void InsertPoolElementWithID(Pool* pool, ID element_id, T data)
    {
        NIT_CHECK_MSG(pool->type_name == typeid(T).name(), "Type mismatch!");
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
        NIT_CHECK_MSG(pool->type_name == typeid(T).name(), "Type mismatch!");
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
        
        using SetDataFunc = void  (*) (void*, u32, void*);
        using GetDataFunc = void* (*) (void*, u32);

        const char*    type_name           = "Invalid";
        void*          elements            = nullptr;
        SetDataFunc    set_data            = nullptr;
        GetDataFunc    get_data            = nullptr;
        u32*           element_id_to_index = nullptr;
        ID*            index_to_element_id = nullptr;
        u32            count               = 0;
        u32            max                 = 0;
        Queue<ID>      available_ids       = {};
        bool           self_id_management  = false;
    };

    template<typename T>
    void InitPool(FastPool* pool, u32 max_element_count = DEFAULT_POOL_ELEMENT_COUNT, bool self_id_management = true)
    {
        pool->type_name = typeid(T).name();
        pool->elements  = new T[max_element_count];

        pool->element_id_to_index = new u32[max_element_count];
        pool->index_to_element_id = new ID[max_element_count];
        
        FillRaw(pool->element_id_to_index, max_element_count, FastPool::INVALID_INDEX);
        FillRaw(pool->index_to_element_id, max_element_count, 0);
        
        pool->set_data = [](void* elements, u32 index, void* data) {
            NIT_CHECK_MSG(elements, "Invalid element array!");
            NIT_CHECK_MSG(data, "Invalid data!");
            T* casted_data     = static_cast<T*>(data);
            T* casted_elements = static_cast<T*>(elements);
            casted_elements[index] = *casted_data;
        };
        
        pool->get_data = [](void* elements, u32 index) -> void* {
            NIT_CHECK_MSG(elements, "Invalid element array!");
            T* casted_elements = static_cast<T*>(elements);
            T* data = &casted_elements[index];
            NIT_CHECK_MSG(data, "Invalid data!");
            return data;
        };

        pool->max = max_element_count;
        
        if (self_id_management)
        {
            for (ID i = 0; i < max_element_count; ++i)
            {
                pool->available_ids.push(i);
            }
        }
        
        pool->self_id_management = self_id_management;
    }

    template<typename T>
    void FinishPool(FastPool* pool)
    {
        NIT_CHECK_MSG(pool, "Invalid pool!");
        pool->type_name = "Invalid";
        T* casted_elements = static_cast<T*>(pool->elements);
        delete[] casted_elements;
        pool->elements = nullptr;
        delete[] pool->index_to_element_id;
        pool->index_to_element_id = nullptr;
        delete[] pool->element_id_to_index;
        pool->element_id_to_index = nullptr;
        pool->count = 0;
        pool->max = 0;
    }
    
    void InsertPoolElementRawWithID(FastPool* pool, ID element_id, void* data);

    template<typename T>
    void InsertPoolElementWithID(FastPool* pool, ID element_id, T data)
    {
        NIT_CHECK_MSG(pool->type_name == typeid(T).name(), "Type mismatch!");
        InsertPoolElementRawWithID(pool, element_id, &data);
    }

    template<typename T>
    void InsertPoolElement(FastPool* pool, ID& out_id, const T& data)
    {
        NIT_CHECK_MSG(pool->self_id_management, "This pool does not manage own ids!");
        out_id = pool->available_ids.front();
        pool->available_ids.pop();
        InsertPoolElementWithID(pool, out_id, data);
    }
    
    void RemovePoolElement(FastPool* pool, ID element_id);

    bool IsPoolElementValid(const FastPool* pool, ID element_id);

    void* GetPoolElementRawPtr(const FastPool* pool, ID element_id);

    template<typename T>
    T* GetPoolElementPtr(const FastPool* pool, ID element_id)
    {
        NIT_CHECK_MSG(pool->type_name == typeid(T).name(), "Type mismatch!");
        return static_cast<T*>(GetPoolElementRawPtr(pool, element_id));
    }

    template<typename T>
    T& GetPoolElement(const FastPool* pool, ID element_id)
    {
        return *GetPoolElementPtr<T>(pool, element_id);
    }
}