#include "nit.h"

using namespace Nit;

struct GameData
{
    SharedPtr<Texture2D> texture;
    SharedPtr<Font>      font;
};

Transform& GetCameraTransform()
{
    auto& camera_group = GetEntityGroup<Camera, Transform>();

    if (camera_group.entities.empty())
    {
        NIT_CHECK(false);
        static Transform def;
        return def;
    }

    Entity main_camera = *camera_group.entities.begin();
    auto& camera_transform = GetComponent<Transform>(main_camera);
    return camera_transform;
}

Entity circle;
Entity cat_entity;

void GameStart()
{
    auto& game_data = AddComponent<GameData>(CreateEntity());
    
    game_data.texture = CreateSharedPtr<Texture2D>("assets/bola.jpg"); 
    game_data.font    = CreateSharedPtr<Font>("assets/AlbertSans-VariableFont_wght.ttf");
    
    Entity camera_entity = CreateEntity();
    AddComponent<Camera>(camera_entity);
    AddComponent<Transform>(camera_entity);
    
    cat_entity = CreateEntity();
    AddComponent<Sprite>(cat_entity).texture = game_data.texture;
    AddComponent<Transform>(cat_entity).position = V3_RIGHT * 2.f;
    
    Entity quad = CreateEntity();
    AddComponent<Sprite>(quad).tint = V4_COLOR_LIGHT_RED;
    AddComponent<Transform>(quad);

    circle = CreateEntity();
    AddComponent<Circle>(circle).tint = V4_COLOR_LIGHT_GREEN;
    AddComponent<Transform>(circle).position = V3_DOWN;

    Entity line = CreateEntity();
    AddComponent<Line2D>(line).tint = V4_COLOR_CYAN;
    AddComponent<Transform>(line).position = V3_LEFT;

    Entity text = CreateEntity();
    AddComponent<Text>(text).text = "UWU";
    GetComponent<Text>(text).tint = V4_COLOR_MAGENTA;
    GetComponent<Text>(text).font = game_data.font;
    
    AddComponent<Transform>(text).position = V3_RIGHT * 2.f;
    
    GetCameraTransform().position = V3_FRONT * 3.f;
}

void GameUpdate()
{
    // Move camera back
    //GetCameraTransform().position += V3_FRONT * 3.f * app->delta_seconds;
}

void DrawImGUI()
{
    ImGui::Begin("Camera");
    auto& camera_transform = GetCameraTransform();
    ImGui::DragFloat3("Position", &camera_transform.position.x, .1f);
    ImGui::DragFloat3("Rotation", &camera_transform.rotation.x);
    ImGui::DragFloat("Cat Alpha", &GetComponent<Sprite>(cat_entity).tint.w, 0.001f);
    ImGui::Checkbox("Use dock space", &app->im_gui_renderer.use_dockspace);
    ImGui::End();
}

void OnRun()
{
    CreateDrawSystem();

    RegisterComponentType<GameData>();
    CreateEntityGroup<GameData>();
    
    //Create game system
    CreateSystem("Game", 1);
    SetSystemCallback(GameStart,   Stage::Start);
    SetSystemCallback(GameUpdate,  Stage::Update);
    SetSystemCallback(DrawImGUI,   Stage::DrawImGUI);
}

struct Dummy
{
    String name = "Alex";
};

int main(int argc, char** argv)
{
    Pool pool;
    InitPool<Dummy>(&pool, 300);

    ID id;
    InsertPoolElement(&pool, id, Dummy{ "Fer" });

    Dummy& data = GetPoolElement<Dummy>(&pool, id);
    NIT_LOG_TRACE("Data is %s", data.name.c_str());
    //console output: Data is Fer
    
    // App app_instance;
    // app_instance.im_gui_enabled = true;
    // //app_instance.im_gui_renderer.show_demo_window = true;
    // SetAppInstance(&app_instance);
    // RunApp(OnRun);
}