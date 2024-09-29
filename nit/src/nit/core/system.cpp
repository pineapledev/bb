#include "system.h"

namespace Nit
{
    Pair<bool, u32> FindSystemByName(System* systems, u32 count, const String& name)
    {
        NIT_CHECK(systems);
        NIT_CHECK_MSG(!name.empty(), "System name invalid!");
        
        for (u32 i = 0; i < count; ++i)
        {
            System* system = systems +i;
            if (system->name == name)
            {
                return {true, i};
            }
        }
        return {false, 0};
    }
    
    void CreateSystem(System* systems, u32& next, const String& name, u32 priority, ExecutionContext context, bool start_disabled)
    {
#ifdef NIT_ENABLE_CHECKS
        NIT_CHECK(systems);
        auto [exists, index] = FindSystemByName(systems, MAX_SYSTEMS, name);
        NIT_CHECK_MSG(!exists, "System with the same name already exists at index %u", index);
#endif
        System& system  = systems[next];
        system.name     = name;
        system.priority = priority;
        system.context  = context;
        system.enabled  = !start_disabled;
        ++next;
    }

    void SetSystemCallback(System& system, VoidFunc callback, Stage stage, bool try_invoke)
    {
        system.callbacks[(u32) stage] = callback;
        
        if (try_invoke)
        {
            TryInvokeSystem(system, stage);
        }
    }
    
    void TryInvokeSystem(const System& system, Stage stage, bool force_enabled)
    {
        NIT_CHECK(stage < Stage::Count);
        const auto callback = system.callbacks[(u8) stage];
        if (callback && (system.enabled || force_enabled))
        {
            callback();
        }
    }
    
    void InvokeSystemCallbacks(System* systems, u32 count, bool pause_mode, Stage stage, bool force_enabled)
    {
        for (u32 i = 0; i < count; ++i)
        {
            System* system = systems + i;
            NIT_CHECK(system);
            
            if (system->context != ExecutionContext::Always && pause_mode)
            {
                continue;
            }
            
            TryInvokeSystem(*system, stage, force_enabled);
        }
    }
}
