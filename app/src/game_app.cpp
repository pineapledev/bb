#include "nit.h"
#include "nit/editor/editor_utils.h"

using namespace Nit;

void OnApplicationRun();
void GameStart();
void GameUpdate();

int main(int argc, char** argv)
{
    App app_instance;
    SetAppInstance(&app_instance);
    RunApp(OnApplicationRun);
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

void OnApplicationRun()
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
    AssetHandle test_scene = FindAssetByName("test_scene");

    if (IsAssetValid(test_scene))
    {
        LoadAsset(test_scene);
    }

    test_texture = FindAssetByName("test_sheet");
}

void GameUpdate()
{
    SpawnEntity();
    
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
            transform.position += ToVector3(move.velocity * app->delta_seconds);
        }
    }
}