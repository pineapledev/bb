#pragma once

namespace Nit
{
    struct MapSparseSet
    {
        static constexpr u32 INVALID_INDEX = 0;
        
        Map<ID, u32> sparse;
        Map<u32, ID> dense;
        u32  count  = 0;
    };
    
    bool IsValid(MapSparseSet* sparse_set);
    bool IsEmpty(MapSparseSet* sparse_set);
    u32  Insert(MapSparseSet* sparse_set, ID element);
    bool Test(MapSparseSet* sparse_set, ID element);
    u32  Search(MapSparseSet* sparse_set, ID element);
    SparseSetDeletion Delete(MapSparseSet* sparse_set, ID element);
    void Free(MapSparseSet* sparse_set);
}