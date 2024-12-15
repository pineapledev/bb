#pragma once
#include "nit/entity/entity.h"

namespace nit
{
    struct TriggerEnterArgs
    {
        EntityID trigger_entity;
        EntityID visitor_entity;
    };

    struct TriggerExitArgs
    {
        EntityID trigger_entity;
        EntityID visitor_entity;
    };
    
    struct TriggerEvents
    {
        Event<const TriggerEnterArgs&> enter_event;
        Event<const TriggerExitArgs&>  exit_event;
    };

    void register_trigger_events_component();
}
