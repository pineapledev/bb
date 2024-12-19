#include "bullet.h"
#include "nit.h"
#include "game.h"
#include "health.h"
#include "movement.h"
#include "player.h"

using namespace nit;

static void game_init();

int main(int argc, char** argv)
{
    game = new Game(); // Heap allocate the game struct in order to avoid stack overflow if the struct grows
    engine_init(game_init);
}

void game_init()
{
    engine_event(Stage::Start)  += EngineListener::create(game_start);
    engine_event(Stage::Update) += EngineListener::create(game_update);
    
    register_movement_component();
    register_health_component();
    register_player_component();
    register_bullet_component();
}