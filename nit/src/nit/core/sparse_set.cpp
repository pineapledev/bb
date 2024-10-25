#include "sparse_set.h"

namespace Nit
{
    bool IsLoaded(SparseSet* sparse_set)
    {
        NIT_CHECK(sparse_set);
        return sparse_set->max != 0;
    }

    bool IsValid(SparseSet* sparse_set)
    {
        return sparse_set && IsLoaded(sparse_set);
    }

    bool IsEmpty(SparseSet* sparse_set)
    {
        NIT_CHECK(IsValid(sparse_set));
        return sparse_set->count == 0;
    }

    bool IsFull(SparseSet* sparse_set)
    {
        NIT_CHECK(IsValid(sparse_set));
        return sparse_set->count == sparse_set->max;
    }

    void Load(SparseSet* sparse_set, u32 max)
    {
        NIT_CHECK(sparse_set && !IsLoaded(sparse_set) && max < SparseSet::INVALID_INDEX);
        sparse_set->max = max;
        sparse_set->sparse = new u32[max];
        sparse_set->dense  = new u32[max];
        memset(sparse_set->sparse, SparseSet::INVALID_INDEX, sizeof(u32) * max);
    }
    
    u32 Insert(SparseSet* sparse_set, u32 element)
    {
        NIT_CHECK(IsValid(sparse_set) && !IsFull(sparse_set));
        NIT_CHECK(!Test(sparse_set, element));
        
        u32 next_slot = sparse_set->count;
        sparse_set->sparse[element] = next_slot;
        sparse_set->dense[next_slot] = element;
        ++sparse_set->count;
        return next_slot;
    }

    bool Test(SparseSet* sparse_set, u32 element)
    {
        NIT_CHECK(IsValid(sparse_set));
        return element < sparse_set->max && sparse_set->sparse[element] != SparseSet::INVALID_INDEX;
    }

    u32 Search(SparseSet* sparse_set, u32 element)
    {
        NIT_CHECK(IsValid(sparse_set));

        if (element >= sparse_set->max)
        {
            return SparseSet::INVALID_INDEX;
        }

        u32 dense_index = sparse_set->sparse[element];
        
        if (dense_index == SparseSet::INVALID_INDEX)
        {
            return SparseSet::INVALID_INDEX;
        }
        
        return dense_index;
    }
    
    void Delete(SparseSet* sparse_set, u32 element)
    {
        NIT_CHECK(IsValid(sparse_set) && sparse_set->count > 0);
        NIT_CHECK(Test(sparse_set, element));
        
        u32 deleted_slot = sparse_set->sparse[element];
        u32 last_slot = sparse_set->count - 1;
        
        sparse_set->sparse[element] = SparseSet::INVALID_INDEX;
        --sparse_set->count;

        u32 last_element = sparse_set->dense[last_slot];
        sparse_set->dense[deleted_slot] = last_element;
        
        sparse_set->sparse[last_element] = deleted_slot;
    }

    void Resize(SparseSet* sparse_set, u32 new_max)
    {
        NIT_CHECK(IsValid(sparse_set) && new_max > sparse_set->max);
        
        u32* new_sparse = new u32[new_max];
        u32* new_dense  = new u32[new_max];
    
        std::copy_n(sparse_set->sparse, sparse_set->max, new_sparse);
        memset(new_sparse + sparse_set->max, SparseSet::INVALID_INDEX, (new_max - sparse_set->max) * sizeof(u32));
        
        std::copy_n(sparse_set->dense, sparse_set->max, new_dense);
    
        delete[] sparse_set->sparse;
        delete[] sparse_set->dense;
    
        sparse_set->sparse = new_sparse;
        sparse_set->dense  = new_dense;
        sparse_set->max    = new_max;
    }

    void Free(SparseSet* sparse_set)
    {
        NIT_CHECK(IsValid(sparse_set));
        
        if (sparse_set->sparse)
        {
            delete[] sparse_set->sparse;
            sparse_set->sparse = nullptr;
        }
        if (sparse_set->dense)
        {
            delete[] sparse_set->dense;
            sparse_set->dense = nullptr;
        }
        
        sparse_set->count = sparse_set->max = 0;
    }
}