#include "bb_pch.h"
#include "health.h"
#include "nit.h"
#include "nit/editor/editor_utils.h"
using namespace nit;

void health_serialize(const Health* health, YAML::Emitter& emitter)
{
    emitter << YAML::Key << "max" << YAML::Value << health->max;
    emitter << YAML::Key << "current" << YAML::Value << health->current;
}

void health_deserialize(Health* health, const YAML::Node& node)
{
    health->max = node["max"].as<float>();
    health->current = node["current"].as<float>();
}

#ifdef NIT_EDITOR_ENABLED
void draw_editor(Health* health)
{

    editor_draw_drag_f32("Max", health->max);
    editor_draw_drag_f32("Current", health->current);

}
#endif

void register_health_component()
{
    TypeArgs<Health> args;
    args.fn_serialize = health_serialize;
    args.fn_deserialize = health_deserialize;
    args.fn_draw_editor = draw_editor;
    component_register<Health>(args);
}

void hittable_serialize(const Hittable* hittable, YAML::Emitter& emitter)
{
}

void hittable_deserialize(Hittable* hittable, const YAML::Node& node)
{
}

ListenerAction on_hittable_trigger_enter(const TriggerEnterArgs& args)
{
    return ListenerAction::StayListening;
}

ListenerAction on_component_added(const ComponentAddedArgs& args)
{
    if (entity_has<Hittable>(args.entity) && entity_has<TriggerEvents>(args.entity))
    {
        auto& trigger_events = entity_get<TriggerEvents>(args.entity);
        trigger_events.enter_event += Listener<const TriggerEnterArgs&>::create(on_hittable_trigger_enter);
    }
    return ListenerAction::StayListening;
}

ListenerAction on_component_removed(const ComponentRemovedArgs& args)
{
    if (entity_has<Hittable>(args.entity))
    {
        
    }
    return ListenerAction::StayListening;
}

void register_hittable_component()
{
    component_register<Hittable>({
        .fn_serialize = hittable_serialize,
        .fn_deserialize = hittable_deserialize,
    });

    engine_get_instance()->entity_registry.component_added_event   += ComponentAddedListener::create(on_component_added);
    engine_get_instance()->entity_registry.component_removed_event += ComponentRemovedListener::create(on_component_removed);
}

void hittable_start()
{
    
}
