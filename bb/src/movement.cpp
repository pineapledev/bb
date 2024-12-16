#include "bb_pch.h"
#include "movement.h"
#include "nit.h"
#include "nit/editor/editor_utils.h"
using namespace nit;

void serialize(const Movement* movement, YAML::Emitter& emitter)
{
    emitter << YAML::Key << "speed" << YAML::Value << movement->speed;
}

void deserialize(Movement* movement, const YAML::Node& node)
{
    movement->speed = node["speed"].as<Vector2>();
}

#ifdef NIT_EDITOR_ENABLED
void draw_editor(Movement* movement)
{
    editor_draw_drag_vector2("Speed", movement->speed);
}
#endif
void register_movement_component()
{
    TypeArgs<Movement> args;
    args.fn_serialize = serialize;
    args.fn_deserialize = deserialize;
    args.fn_draw_editor = draw_editor;
    component_register<Movement>(args);
}