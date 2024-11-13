#include "nit.h"
#include "nit/editor/editor_utils.h"

using namespace nit;

ListenerAction on_run();
ListenerAction game_start();
ListenerAction game_update();

int main(int argc, char** argv)
{
    Engine engine_instance;
    engine_set_instance(&engine_instance);
    engine_event(Stage::Run) += EngineListener::Create(on_run);
    engine_run();
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
    entity::add<Transform>(entity).position = position;
    //entity::add<Transform>(entity).position = V3_ZERO;
    entity::add<Sprite>(entity).tint = GetRandomColor();
    //entity::get<Sprite>(entity).texture = test_texture;
    SetSpriteSubTexture2D(entity::get<Sprite>(entity), "cpp");
    reset_movement(entity::get<Transform>(entity), entity::add<Move>(entity));
}

// -----------------------------------------------------------------

ListenerAction on_run()
{
    //Create game system
    engine_event(Stage::Start)  += EngineListener::Create(game_start);
    engine_event(Stage::Update) += EngineListener::Create(game_update);

    RegisterComponentType<Move>();
    entity::create_group<Transform, Sprite, Move>();

    return ListenerAction::StayListening;
}

// -----------------------------------------------------------------

ListenerAction game_start()
{
    AssetHandle test_scene = asset_find_by_name("test_scene");

    if (asset_valid(test_scene))
    {
        asset_load(test_scene);
    }

    test_texture = asset_find_by_name("test_sheet");

    return ListenerAction::StayListening;
}

ListenerAction game_update()
{
    //SpawnEntity();
    
    for (Entity entity : entity::get_group<Transform, Sprite, Move>().entities)
    {
        auto& transform = entity::get<Transform>(entity);
        auto& move = entity::get<Move>(entity);

        if (Distance(ToVector2(transform.position), move.destination) < 0.1f)
        {
            reset_movement(transform, move);
        }
        else
        {
            transform.position += ToVector3(move.velocity * engine_get_instance()->delta_seconds);
        }
    }

    return ListenerAction::StayListening;
}