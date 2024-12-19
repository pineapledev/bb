#include "player.h"

#include "bullet.h"
#include "game.h"
#include "movement.h"

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
    editor_draw_drag_f32("L.1 GunRate", player->firerate_gun_1);
    editor_draw_drag_f32("L.2 GunRate", player->firerate_gun_2);
    editor_draw_drag_f32("L.3 GunRate", player->firerate_gun_3);
    editor_draw_drag_f32("L.MAX GunRate", player->firerate_gun_max);
    editor_draw_drag_f32("L.1 SwordRate", player->firerate_sword_1);
    editor_draw_drag_f32("L.2 SwordRate", player->firerate_sword_2);
    editor_draw_drag_f32("L.3 SwordRate", player->firerate_sword_3);
    editor_draw_drag_f32("L.MAX SwordRate", player->firerate_sword_max);
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

static ListenerAction input_callback_move(const InputActionContext& context);
static ListenerAction input_callback_shoot(const InputActionContext& context);

void player_start()
{
    auto& player = entity_get<Player>(game->entity_player);
    
    // Input assets
    {
        player.input_move = asset_find_by_name(INPUT_NAME_PLAYER_MOVEMENT);

        if (asset_valid(player.input_move))
        {
            InputAction* input = asset_get_data<InputAction>(player.input_move);
            input->input_performed_event += InputListener::create(input_callback_move);
        }

        player.input_shoot = asset_find_by_name(INPUT_NAME_PLAYER_SHOOT);
    
        if (asset_valid(player.input_move))
        {
            InputAction* input = asset_get_data<InputAction>(player.input_shoot);
            input->input_performed_event += InputListener::create(input_callback_shoot);
        }
    }
}

void player_update()
{
}

ListenerAction input_callback_move(const InputActionContext& context)
{
    if (!entity_valid(game->entity_player))
    {
        NIT_CHECK(false);
        return ListenerAction::StopListening;
    }
    
    Vector2 input_value = (const Vector2&) context.inputValue;

    auto dead_zone_lambda = [](const f32 axis_val)
    {
        return min(1.f, (max(0.f, abs(axis_val) - 0.1f) / (1.f - 0.1f))) * sign(axis_val);
    };

    Vector2 new_value = Vector2(input_value.x, input_value.y);
    new_value = new_value.x == 0.f && new_value.y == 0.f ? new_value : normalize(new_value);
    new_value = new_value * dead_zone_lambda(magnitude(input_value));
    input_value.x = new_value.x;
    input_value.y = new_value.y;
        
    entity_get<Transform>(game->entity_player).position += to_v3(multiply(entity_get<Movement>(game->entity_player).speed, input_value) * delta_seconds());
    
    return ListenerAction::StayListening;
}

ListenerAction input_callback_shoot(const InputActionContext& context)
{
    bullet_spawn(entity_get<Transform>(game->entity_player).position);
    return ListenerAction::StayListening;
}