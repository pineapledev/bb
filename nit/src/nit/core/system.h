#pragma once

namespace Nit
{
    inline static constexpr u32 MAX_SYSTEMS = 100;
    
    enum class ExecutionContext : u8
    {
        Runtime
      , Editor
      , Always
    };
    
    enum class Stage : u8
    {
        Run
      , Start
      , Update
      , FixedUpdate
      , Draw
      , DrawImGUI
      , End
      , Count
    };

    struct System
    {
        String           name;
        u32              priority          = 0;
        ExecutionContext context           = ExecutionContext::Runtime;
        bool             enabled           = false;
        VoidFunc         callbacks[(u8) Stage::Count];
    };
    
    struct SystemStack
    {
        u32     next_system     = 0;
        System  systems[MAX_SYSTEMS];
    };
    
    Pair<bool, u32> FindSystemByName(const String& name);
    void CreateSystem(System* systems, u32& next, const String& name, u32 priority = 0, ExecutionContext context = ExecutionContext::Runtime, bool start_disabled = false);
    void SetSystemCallback(System& system, VoidFunc callback, Stage stage, bool try_invoke = false);
    void TryInvokeSystem(const System& system, Stage stage, bool force_enabled = false);
    void InvokeSystemCallbacks(System* systems, u32 count, bool pause_mode, Stage stage, bool force_enabled = false);
}