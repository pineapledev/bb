#pragma once

namespace Nit
{
    struct Pool
    {
        using SetDataFunc = void  (*) (void*, u32, void*);
        using GetDataFunc = void* (*) (void*, u32);

        const char*    type_name           = "";
        void*          elements            = nullptr;
        SetDataFunc    set_data            = nullptr;
        GetDataFunc    get_data            = nullptr;
        Map<ID,  u32>  element_id_to_index = {};
        Map<u32, ID>   index_to_element_id = {};
        u32            count               = 0;
        u32            max                 = 0;
    };

    template<typename T>
    Pool* CreatePool(u32 max_element_count = 30)
    {
        Pool* pool = new Pool();
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
        return pool;
    }
    
    template<typename T>
    void DestroyPool(Pool* pool)
    {
        NIT_CHECK_MSG(pool, "Invalid array!");
        delete[] static_cast<T*>(pool->elements);
        delete pool;
    }
    
    ID    InsertPoolElement(Pool* pool, void* data, ID element_id);
    void  RemovePoolElement(Pool* pool, ID element_id);
    void* GetPoolElement(Pool* pool, ID element_id);
}