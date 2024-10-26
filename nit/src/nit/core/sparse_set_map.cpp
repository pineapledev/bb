#include "sparse_set_map.h"

namespace Nit
{
    bool IsValid(SparseSetMap* sparse_set)
    {
        return sparse_set;
    }

    bool IsEmpty(SparseSetMap* sparse_set)
    {
        NIT_CHECK(IsValid(sparse_set));
        return sparse_set->count == 0;
    }
    
    u32 Insert(SparseSetMap* sparse_set, ID element)
    {
        NIT_CHECK(IsValid(sparse_set));
        NIT_CHECK(element != SparseSetMap::INVALID_ID)
        NIT_CHECK(!Test(sparse_set, element));
        
        u32 next_slot = sparse_set->count;
        sparse_set->sparse[element] = next_slot;
        sparse_set->dense[next_slot] = element;
        ++sparse_set->count;
        
        return next_slot;
    }

    bool Test(SparseSetMap* sparse_set, ID element)
    {
        NIT_CHECK(IsValid(sparse_set));
        return sparse_set->sparse.count(element) > 0;
    }

    u32 Search(SparseSetMap* sparse_set, ID element)
    {
        NIT_CHECK(IsValid(sparse_set));
        
        if (!Test(sparse_set, element))
        {
            return SparseSetMap::INVALID_INDEX;
        }
        
        return sparse_set->sparse[element];
    }
    
    SparseSetDeletion Delete(SparseSetMap* sparse_set, ID element)
    {
        NIT_CHECK(IsValid(sparse_set) && sparse_set->count > 0);
        NIT_CHECK(Test(sparse_set, element));
        
        u32 deleted_slot = sparse_set->sparse[element];
        u32 last_slot = sparse_set->count - 1;

        sparse_set->sparse.erase(element);
        --sparse_set->count;

        ID last_element = sparse_set->dense[last_slot];
        sparse_set->dense.erase(last_slot);
        sparse_set->dense[deleted_slot] = last_element;
        sparse_set->sparse[last_element] = deleted_slot;

        return { deleted_slot, last_slot };
    }
}