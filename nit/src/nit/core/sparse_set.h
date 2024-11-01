#pragma once

namespace Nit
{
    struct SparseSet
    {
        static constexpr u32 INVALID = U32_MAX;
        
        u32* sparse = nullptr;
        u32* dense  = nullptr;
        u32  count  = 0;
        u32  max    = 0;
    };
    
    struct SparseSetDeletion
    {
        bool succeded     = false;
        u32  deleted_slot = 0;
        u32  last_slot    = 0;
    };
    
    bool IsValid(SparseSet* sparse_set);
    bool IsEmpty(SparseSet* sparse_set);
    bool IsFull(SparseSet* sparse_set);
    void Load(SparseSet* sparse_set, u32 max);
    u32  Insert(SparseSet* sparse_set, u32 element);
    bool Test(SparseSet* sparse_set, u32 element);
    u32  Search(SparseSet* sparse_set, u32 element);
    SparseSetDeletion Delete(SparseSet* sparse_set, u32 element);
    void Resize(SparseSet* sparse_set, u32 new_max);
    void Free(SparseSet* sparse_set);
}