#include "game.h"

using namespace nit;

ListenerAction input_callback_player_move(const InputActionContext& context)
{
    if (!entity_valid(game->player))
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
        
    entity_get<Transform>(game->player).position += to_v3(multiply(entity_get<Movement>(game->player).speed, input_value) * delta_seconds());
    
    return ListenerAction::StayListening;
}

ListenerAction game_start()
{
    game->test_scene = asset_find_by_name(SCENE_NAME_GAME);
    
    if (asset_valid(game->test_scene))
    {
        asset_load(game->test_scene);
    }
    
    game->player_input_move = asset_find_by_name(INPUT_NAME_PLAYER_MOVEMENT);

    if (asset_valid(game->player_input_move))
    {
        InputAction* input = asset_get_data<InputAction>(game->player_input_move);
        input->input_performed_event += InputListener::create(input_callback_player_move);
    }
    
    game->player = entity_find_by_name(ENTITY_NAME_PLAYER);
    
    return ListenerAction::StayListening;
}

ListenerAction game_update()
{
    return ListenerAction::StayListening;
}