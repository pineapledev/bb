#include "bb_pch.h"
#include "player.h"
#include "nit.h"
#include "nit/editor/editor_utils.h"
using namespace nit;

void serialize(const Player* player, YAML::Emitter& emitter)
{
    emitter << YAML::Key << "firerate_gun_1"    << YAML::Value << player->firerate_gun_1;
    emitter << YAML::Key << "firerate_gun_2"    << YAML::Value << player->firerate_gun_2;
    emitter << YAML::Key << "firerate_gun_3"    << YAML::Value << player->firerate_gun_3;
    emitter << YAML::Key << "firerate_gun_max"    << YAML::Value << player->firerate_gun_max;
    emitter << YAML::Key << "firerate_sword_1"    << YAML::Value << player->firerate_sword_1;
    emitter << YAML::Key << "firerate_sword_2"    << YAML::Value << player->firerate_sword_2;
    emitter << YAML::Key << "firerate_sword_3"    << YAML::Value << player->firerate_sword_3;
    emitter << YAML::Key << "firerate_sword_max"    << YAML::Value << player->firerate_sword_max;
}

void deserialize(Player* player, const YAML::Node& node)
{
    player->firerate_gun_1 = node["firerate_gun_1"].as<float>();
    player->firerate_gun_2 = node["firerate_gun_2"].as<float>();
    player->firerate_gun_3 = node["firerate_gun_3"].as<float>();
    player->firerate_gun_max = node["firerate_gun_max"].as<float>();
    player->firerate_sword_1 = node["firerate_sword_1"].as<float>();
    player->firerate_sword_2 = node["firerate_sword_2"].as<float>();
    player->firerate_sword_3 = node["firerate_sword_3"].as<float>();
    player->firerate_sword_max = node["firerate_sword_max"].as<float>();
}

#ifdef NIT_EDITOR_ENABLED
void draw_editor(Player* player)
{
    editor_draw_drag_f32("L1 GunRate", player->firerate_gun_1);
    editor_draw_drag_f32("L2 GunRate", player->firerate_gun_2);
    editor_draw_drag_f32("L3 GunRate", player->firerate_gun_3);
    editor_draw_drag_f32("LMAX GunRate", player->firerate_gun_max);
    editor_draw_drag_f32("L1 SwrdRate", player->firerate_sword_1);
    editor_draw_drag_f32("L2 SwrdRate", player->firerate_sword_2);
    editor_draw_drag_f32("L3 SwrdRate", player->firerate_sword_3);
    editor_draw_drag_f32("LMAX SwrdRate", player->firerate_sword_max);
}
#endif
void register_player_component()
{
    TypeArgs<Player> args;
    args.fn_serialize = serialize;
    args.fn_deserialize = deserialize;
    args.fn_draw_editor = draw_editor;
    component_register<Player>(args);
}