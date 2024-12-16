#include "nit.h"

using namespace nit;

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
    return ListenerAction::StayListening;
}

void init()
{
    engine_event(Stage::Start)  += EngineListener::create(start);
    engine_event(Stage::Update) += EngineListener::create(update);
}

int main(int argc, char** argv)
{
    engine_init(init);
}