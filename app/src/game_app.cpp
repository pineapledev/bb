#include "nit.h"
#include "nit/editor/editor_utils.h"

using namespace nit;

void on_run();
void game_start();
void game_update();

int main(int argc, char** argv)
{
    Engine engine_instance;
    engine::set_instance(&engine_instance);
    engine::run(on_run);
}

// -----------------------------------------------------------------

constexpr Vector2 RECT_LEFT  = { -100.f, 100.f };
constexpr Vector2 RECT_RIGHT = { 100.f, -100.f };
constexpr float MIN_SPEED = 1.f;
constexpr float MAX_SPEED = 15.f;

// -----------------------------------------------------------------


struct Move
{
    Vector2 velocity;
    Vector2 destination;
};

AssetHandle test_texture;

// -----------------------------------------------------------------

void reset_movement(Transform& transform, Move& move)
{
    float speed      = GetRandomValue(MIN_SPEED, MAX_SPEED);
    move.destination = RandomPointInSquare(RECT_LEFT.x, RECT_RIGHT.y, RECT_RIGHT.x, RECT_LEFT.y);
    move.velocity    = Normalize(move.destination - ToVector2(transform.position)) * speed;
}

// -----------------------------------------------------------------

void spawn_entity()
{
    Entity entity = CreateEntity();
    Vector3 position = ToVector3(RandomPointInSquare(RECT_LEFT.x, RECT_RIGHT.y, RECT_RIGHT.x, RECT_LEFT.y)); 
    add_component<Transform>(entity).position = position;
    //add_component<Transform>(entity).position = V3_ZERO;
    add_component<Sprite>(entity).tint = GetRandomColor();
    //get_component<Sprite>(entity).texture = test_texture;
    SetSpriteSubTexture2D(get_component<Sprite>(entity), "cpp");
    reset_movement(get_component<Transform>(entity), add_component<Move>(entity));
}

// -----------------------------------------------------------------

void on_run()
{
    //Create game system
    CreateSystem("Game", 1);
    SetSystemCallback(game_start,   Stage::Start);
    SetSystemCallback(game_update,  Stage::Update);

    RegisterComponentType<Move>();
    CreateEntityGroup<Transform, Sprite, Move>();
}

// -----------------------------------------------------------------

void game_start()
{
    AssetHandle test_scene = find_asset_by_name("test_scene");

    if (is_asset_valid(test_scene))
    {
        load_asset(test_scene);
    }

    test_texture = find_asset_by_name("test_sheet");
}

void game_update()
{
    //SpawnEntity();
    
    for (Entity entity : GetEntityGroup<Transform, Sprite, Move>().entities)
    {
        auto& transform = get_component<Transform>(entity);
        auto& move = get_component<Move>(entity);

        if (Distance(ToVector2(transform.position), move.destination) < 0.1f)
        {
            reset_movement(transform, move);
        }
        else
        {
            transform.position += ToVector3(move.velocity * engine::get_instance()->delta_seconds);
        }
    }
}