#pragma once
#include "nit/entity/entity.h"

namespace nit
{
    struct TriggerEnterEvent
    {
        EntityID trigger_entity;
        EntityID visitor_entity;
    };

    struct TriggerExitEvent
    {
        EntityID trigger_entity;
        EntityID visitor_entity;
    };
    
    struct TriggerEvents
    {
        Array<TriggerEnterEvent> enter_events;
        Array<TriggerExitEvent>  exit_events;
    };

    void register_trigger_events_component();
}
