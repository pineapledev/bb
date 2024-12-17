#include "health.h"
#include "movement.h"
#include "nit.h"
#include "player.h"
#include "nit/editor/editor_utils.h"

using namespace nit;

AssetHandle test_scene;
EntityID player;
AssetHandle player_input_move;

ListenerAction player_input_move_callback(const InputActionContext& context)
{
    if (entity_valid(player))
    {
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
        
        entity_get<Transform>(player).position += to_v3(multiply(entity_get<Movement>(player).speed, input_value) * delta_seconds());
    }
    
    return ListenerAction::StayListening;
}

ListenerAction start()
{
    test_scene = asset_find_by_name("GameScene");
    
    if (asset_valid(test_scene))
    {
        asset_load(test_scene);
    }

    player = entity_find_by_name("Player");

    player_input_move = asset_find_by_name("PlayerMoveInput");
    asset_get_data<InputAction>(player_input_move)->input_performed_event += Listener<const InputActionContext&>::create(player_input_move_callback);
    
    return ListenerAction::StayListening;
}

ListenerAction update()
{
    return ListenerAction::StayListening;
}


void init()
{
    engine_event(Stage::Start)  += EngineListener::create(start);
    engine_event(Stage::Update) += EngineListener::create(update);
    register_movement_component();
    register_health_component();
    register_player_component();
}

int main(int argc, char** argv)
{
    engine_init(init);
}