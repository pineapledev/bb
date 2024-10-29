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

struct Move
{
    Vector2 velocity;
    Vector2 destination;
};

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
}

void GameUpdate()
{
}