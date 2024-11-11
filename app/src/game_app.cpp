#include "nit.h"
#include "nit/editor/editor_utils.h"

using namespace nit;

void OnRun();
void GameStart();
void GameUpdate();

int main(int argc, char** argv)
{
    Engine engine_instance;
    engine::set_instance(&engine_instance);
    engine::run(OnRun);
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

void ResetMovement(Transform& transform, Move& move)
{
    float speed      = GetRandomValue(MIN_SPEED, MAX_SPEED);
    move.destination = RandomPointInSquare(RECT_LEFT.x, RECT_RIGHT.y, RECT_RIGHT.x, RECT_LEFT.y);
    move.velocity    = Normalize(move.destination - ToVector2(transform.position)) * speed;
}

// -----------------------------------------------------------------

void SpawnEntity()
{
    Entity entity = CreateEntity();
    Vector3 position = ToVector3(RandomPointInSquare(RECT_LEFT.x, RECT_RIGHT.y, RECT_RIGHT.x, RECT_LEFT.y)); 
    AddComponent<Transform>(entity).position = position;
    //AddComponent<Transform>(entity).position = V3_ZERO;
    AddComponent<Sprite>(entity).tint = GetRandomColor();
    //GetComponent<Sprite>(entity).texture = test_texture;
    SetSpriteSubTexture2D(GetComponent<Sprite>(entity), "cpp");
    ResetMovement(GetComponent<Transform>(entity), AddComponent<Move>(entity));
}

// -----------------------------------------------------------------

void OnRun()
{
    //Create game system
    CreateSystem("Game", 1);
    SetSystemCallback(GameStart,   Stage::Start);
    SetSystemCallback(GameUpdate,  Stage::Update);

    RegisterComponentType<Move>();
    CreateEntityGroup<Transform, Sprite, Move>();
}

// -----------------------------------------------------------------

void GameStart()
{
    AssetHandle test_scene = find_asset_by_name("test_scene");

    if (is_asset_valid(test_scene))
    {
        load_asset(test_scene);
    }

    test_texture = find_asset_by_name("test_sheet");
}

void GameUpdate()
{
    //SpawnEntity();
    
    for (Entity entity : GetEntityGroup<Transform, Sprite, Move>().entities)
    {
        auto& transform = GetComponent<Transform>(entity);
        auto& move = GetComponent<Move>(entity);

        if (Distance(ToVector2(transform.position), move.destination) < 0.1f)
        {
            ResetMovement(transform, move);
        }
        else
        {
            transform.position += ToVector3(move.velocity * engine::get_instance()->delta_seconds);
        }
    }
}