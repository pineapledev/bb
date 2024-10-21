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
        
        // Not needed, just for debugging clarity
        memset(sparse_set->dense,  SparseSet::INVALID_INDEX, sizeof(u32) * max);
    }
    
    void Insert(SparseSet* sparse_set, u32 element)
    {
        NIT_CHECK(IsValid(sparse_set) && !IsFull(sparse_set));
        
        u32 next_slot = sparse_set->count;
        sparse_set->sparse[element] = next_slot;
        sparse_set->dense[next_slot] = element;
        ++sparse_set->count;
    }

    u32 Search(SparseSet* sparse_set, u32 element)
    {
        NIT_CHECK(IsValid(sparse_set));
        
        if (sparse_set->sparse[element] != SparseSet::INVALID_INDEX && sparse_set->dense[sparse_set->sparse[element]])
        {
            return sparse_set->sparse[element];
        }
        
        return SparseSet::INVALID_INDEX;
    }
    
    void Delete(SparseSet* sparse_set, u32 element)
    {
        NIT_CHECK(IsValid(sparse_set));
        NIT_CHECK(Search(sparse_set, element) != SparseSet::INVALID_INDEX);
        
        u32 deleted_slot = sparse_set->sparse[element];
        u32 last_slot = sparse_set->count - 1;
        
        sparse_set->sparse[element] = SparseSet::INVALID_INDEX;
        --sparse_set->count;

        u32 last_element = sparse_set->dense[last_slot];
        sparse_set->dense[deleted_slot] = last_element;
        sparse_set->sparse[last_element] = sparse_set->dense[deleted_slot];

        // Not needed, just for debugging clarity
        sparse_set->dense[last_slot] = SparseSet::INVALID_INDEX;
    }

    void Free(SparseSet* sparse_set)
    {
        NIT_CHECK(IsValid(sparse_set));
        delete [] sparse_set->sparse;
        delete [] sparse_set->dense;
        *sparse_set = {};
    }
}