#pragma once

namespace Nit
{
    struct SparseSetMap
    {
        static constexpr u32 INVALID_INDEX = 0;
        
        Map<ID, u32> sparse;
        Map<u32, ID> dense;
        u32  count  = 0;
    };
    
    bool IsValid(SparseSetMap* sparse_set);
    bool IsEmpty(SparseSetMap* sparse_set);
    u32  Insert(SparseSetMap* sparse_set, ID element);
    bool Test(SparseSetMap* sparse_set, ID element);
    u32  Search(SparseSetMap* sparse_set, ID element);
    SparseSetDeletion Delete(SparseSetMap* sparse_set, ID element);
    void Free(SparseSetMap* sparse_set);
}