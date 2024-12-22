#include "bb_pch.h"
#include "health.h"

#include "bullet.h"
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
