#include "hittable.h"
#include "bullet.h"
#include "health.h"

void hittable_serialize(const Hittable* hittable, YAML::Emitter& emitter)
{
}

void hittable_deserialize(Hittable* hittable, const YAML::Node& node)
{
}

void register_hittable_component()
{
    component_register<Hittable>({
        .fn_serialize = hittable_serialize,
        .fn_deserialize = hittable_deserialize,
    });

    entity_create_group<TriggerEvents, Hittable>();
}

void hittable_start()
{
    
}

void hittable_update()
{
    for (EntityID entity : entity_get_group<TriggerEvents, Hittable>().entities)
    {
        auto& events = entity_get<TriggerEvents>(entity);

        for (TriggerEnterEvent& enter_event : events.enter_events)
        {
            bool take_damage = entity_has<Health>(enter_event.trigger_entity);
            take_damage     &= entity_has<Bullet>(enter_event.visitor_entity);
            
            if (take_damage)
            {
                Health& health = entity_get<Health>(enter_event.trigger_entity);
                Bullet& bullet = entity_get<Bullet>(enter_event.visitor_entity);
        
                if (health.enabled && bullet.enabled)
                {
                    health.current -= bullet.damage;
                }
        
            }
        }
    }
}
