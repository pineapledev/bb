#include "health.h"
#include "movement.h"
#include "nit.h"
#include "player.h"
#include "nit/editor/editor_utils.h"

using namespace nit;

AssetHandle test_scene;
EntityID player;


ListenerAction start()
{
    test_scene = asset_find_by_name("test scene");
    
    if (asset_valid(test_scene))
    {
        asset_load(test_scene);
    }

    player = entity_find_by_name("Player");
    
    return ListenerAction::StayListening;
}

ListenerAction update()
{
    if (input_is_key_pressed(KeyD))
    {
        //entity_get<Transform>(player).position.x += Player.x_speed * delta_seconds();
    }
    
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