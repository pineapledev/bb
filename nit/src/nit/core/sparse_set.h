#pragma once

namespace nit
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
    
    namespace sparse
    {
        bool is_valid(SparseSet* sparse_set);
        bool is_empty(SparseSet* sparse_set);
        bool is_full(SparseSet* sparse_set);
        void load(SparseSet* sparse_set, u32 max);
        u32  insert(SparseSet* sparse_set, u32 element);
        bool test(SparseSet* sparse_set, u32 element);
        u32  search(SparseSet* sparse_set, u32 element);
        SparseSetDeletion remove(SparseSet* sparse_set, u32 element);
        void resize(SparseSet* sparse_set, u32 new_max);
        void release(SparseSet* sparse_set);
    }
}