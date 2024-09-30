#pragma once

namespace Nit
{
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
    void InitPool(Pool* pool, u32 max_element_count = 30)
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
    
    ID InsertPoolElementRawWithID(Pool* pool, void* data, ID element_id);

    template<typename T>
    ID InsertPoolElementWithID(Pool* pool, const T& data, ID element_id)
    {
        NIT_CHECK_MSG(pool->type_name == typeid(T).name(), "Type mismatch!");
        T data_copy = data;
        return InsertPoolElementRawWithID(pool, &data_copy, element_id);
    }

    template<typename T>
    ID InsertPoolElement(Pool* pool, const T& data)
    {
        return InsertPoolElementWithID(pool, data, GenerateID());
    }
    
    void RemovePoolElement(Pool* pool, ID element_id);

    bool IsPoolElementValid(const Pool* pool, ID element_id);

    void* GetPoolElementRawPtr(Pool* pool, ID element_id);

    template<typename T>
    T* GetPoolElementPtr(Pool* pool, ID element_id)
    {
        NIT_CHECK_MSG(pool->type_name == typeid(T).name(), "Type mismatch!");
        return static_cast<T*>(GetPoolElementRawPtr(pool, element_id));
    }

    template<typename T>
    T& GetPoolElement(Pool* pool, ID element_id)
    {
        return *GetPoolElementPtr<T>(pool, element_id);
    }
}