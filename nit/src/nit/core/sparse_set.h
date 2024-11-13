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

    bool              sparse_is_valid (SparseSet* sparse_set);
    bool              sparse_is_empty (SparseSet* sparse_set);
    bool              sparse_is_full  (SparseSet* sparse_set);
    void              sparse_load     (SparseSet* sparse_set, u32 max);
    u32               sparse_insert   (SparseSet* sparse_set, u32 element);
    bool              sparse_test     (SparseSet* sparse_set, u32 element);
    u32               sparse_search   (SparseSet* sparse_set, u32 element);
    SparseSetDeletion sparse_remove   (SparseSet* sparse_set, u32 element);
    void              sparse_resize   (SparseSet* sparse_set, u32 new_max);
    void              sparse_release  (SparseSet* sparse_set);
}