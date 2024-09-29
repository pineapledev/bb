#include "nit.h"

using namespace Nit;

struct GameData
{
    TSharedPtr<Texture2D> texture;
    TSharedPtr<Font>      font;
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

    TEntity main_camera = *camera_group.entities.begin();
    auto& camera_transform = GetComponent<Transform>(main_camera);
    return camera_transform;
}

void GameStart()
{
    auto& game_data = AddComponent<GameData>(CreateEntity());
    
    game_data.texture = CreateSharedPtr<Texture2D>("assets/bola.jpg"); 
    game_data.font    = CreateSharedPtr<Font>("assets/AlbertSans-VariableFont_wght.ttf");
    
    TEntity camera_entity = CreateEntity();
    AddComponent<Camera>(camera_entity);
    AddComponent<Transform>(camera_entity);
    
    TEntity cat_entity = CreateEntity();
    AddComponent<Sprite>(cat_entity).texture = game_data.texture;
    AddComponent<Transform>(cat_entity).position = V3_RIGHT * 2.f;
    
    TEntity quad = CreateEntity();
    AddComponent<Sprite>(quad);
    AddComponent<Transform>(quad);

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
    ImGui::DragFloat3("Position", &camera_transform.position.x);
    ImGui::DragFloat3("Rotation", &camera_transform.rotation.x);
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

int main(int argc, char** argv)
{
    App app_instance;
    app_instance.im_gui_enabled = true;
    //app_instance.im_gui_renderer.show_demo_window = true;
    SetAppInstance(&app_instance);
    RunApp(OnRun);
}