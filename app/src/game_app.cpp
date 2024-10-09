#include "nit.h"

using namespace Nit;

void OnApplicationRun();
void GameStart();
void GameUpdate();
void DrawImGUI();

int main(int argc, char** argv)
{
    App app_instance;
    app_instance.im_gui_enabled = true;
    //app_instance.im_gui_renderer.show_demo_window = true;
    SetAppInstance(&app_instance);
    RunApp(OnApplicationRun);
}

// -----------------------------------------------------------------

void OnApplicationRun()
{
    //Create game system
    CreateSystem("Game", 1);
    SetSystemCallback(GameStart,   Stage::Start);
    SetSystemCallback(GameUpdate,  Stage::Update);
    SetSystemCallback(DrawImGUI,   Stage::DrawImGUI);
}

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

Entity circle_entity;
Entity cat_entity;

void GameStart()
{
    Entity camera_entity = CreateEntity();
    AddComponent<Camera>(camera_entity);
    AddComponent<Transform>(camera_entity);
    
    cat_entity = CreateEntity();
    Sprite sprite;
    sprite.texture_id = FindAssetByName("bola");
    AddComponent<Sprite>(cat_entity, sprite);
    
    AddComponent<Transform>(cat_entity).position = V3_RIGHT * 2.f;
    
    Entity quad_entity = CreateEntity();
    AddComponent<Sprite>(quad_entity).tint = V4_COLOR_LIGHT_RED;
    AddComponent<Transform>(quad_entity);

    circle_entity = CreateEntity();
    AddComponent<Circle>(circle_entity).tint = V4_COLOR_LIGHT_GREEN;
    AddComponent<Transform>(circle_entity).position = V3_DOWN;

    Entity line_entity = CreateEntity();
    AddComponent<Line2D>(line_entity).tint = V4_COLOR_CYAN;
    AddComponent<Transform>(line_entity).position = V3_LEFT;

    Entity text_entity = CreateEntity();
    Text text;
    text.text = "UWU";
    text.tint = V4_COLOR_MAGENTA;
    text.font_id = FindAssetByName("default_font");
    AddComponent<Text>(text_entity, text);
    AddComponent<Transform>(text_entity).position = V3_RIGHT * 2.f;
    
    GetCameraTransform().position = V3_FRONT * 3.f;

    ID scene_id = CreateAsset<Scene>("test_scene", "assets");
    Scene& scene = GetAssetData<Scene>(scene_id);
    
    scene.entities.push_back(camera_entity);
    scene.entities.push_back(cat_entity);
    scene.entities.push_back(quad_entity);
    scene.entities.push_back(circle_entity);
    scene.entities.push_back(line_entity);
    scene.entities.push_back(text_entity);

    SerializeAssetToFile(scene_id);
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