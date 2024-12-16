#include "nit.h"

using namespace nit;

constexpr Vector2 RECT_LEFT  = { -100.f, 100.f };
constexpr Vector2 RECT_RIGHT = { 100.f, -100.f };
constexpr float MIN_SPEED = 1.f;
constexpr float MAX_SPEED = 15.f;

struct Move
{
    Vector2 velocity;
    Vector2 destination;
};

void reset_movement(Transform& transform, Move& move)
{
    float speed      = random_value(MIN_SPEED, MAX_SPEED);
    move.destination = random_point_in_square(RECT_LEFT.x, RECT_RIGHT.y, RECT_RIGHT.x, RECT_LEFT.y);
    move.velocity    = normalize(move.destination - to_v2(transform.position)) * speed;
}

void spawn_entity()
{
    EntityID entity = entity_create();
    Vector3 position = to_v3(random_point_in_square(RECT_LEFT.x, RECT_RIGHT.y, RECT_RIGHT.x, RECT_LEFT.y)); 
    entity_add<Transform>(entity).position = position;
    entity_add<Sprite>(entity).tint = GetRandomColor();
    sprite_set_sub_texture(entity_get<Sprite>(entity), "cpp");
    reset_movement(entity_get<Transform>(entity), entity_add<Move>(entity));
}

// -----------------------------------------------------------------

void init();

int main(int argc, char** argv)
{
    engine_init(init);
}

// -----------------------------------------------------------------

ListenerAction start();
ListenerAction update();

void init()
{
    //Create game system
    engine_event(Stage::Start)  += EngineListener::create(start);
    engine_event(Stage::Update) += EngineListener::create(update);

    component_register<Move>();
    entity_create_group<Transform, Sprite, Move>();
}

AssetHandle test_scene;

ListenerAction start()
{
    test_scene = asset_find_by_name("test scene");

    if (asset_valid(test_scene))
    {
        asset_load(test_scene);
    }
    
    return ListenerAction::StayListening;
}

ListenerAction update()
{
    spawn_entity();
    
    for (EntityID entity : entity_get_group<Transform, Sprite, Move>().entities)
    {
        auto& transform = entity_get<Transform>(entity);
        auto& move      = entity_get<Move>(entity);
        
        if (distance(to_v2(transform.position), move.destination) < 0.1f)
        {
            reset_movement(transform, move);
        }
        else
        {
            transform.position += to_v3(move.velocity * delta_seconds());
        }
    }

    return ListenerAction::StayListening;
}