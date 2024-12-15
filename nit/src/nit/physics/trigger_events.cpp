#include "trigger_events.h"
#include "entity/entity.h"

namespace nit
{
    void serialize(const TriggerEvents* collider, YAML::Emitter& emitter)
    {
    }
    
    void deserialize(TriggerEvents* collider, const YAML::Node& node)
    {
    }

    void register_trigger_events_component()
    {
        component_register<TriggerEvents>({
            .fn_serialize   = serialize,
            .fn_deserialize = deserialize,
        });
    }
}
