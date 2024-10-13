#include "nit.h"

using namespace Nit;

void OnApplicationRun();
void GameStart();
void GameUpdate();
void DrawImGUI();

int main(int argc, char** argv)
{
    App app_instance;
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

void GameStart()
{
    ID test_scene_id = FindAssetByName("test_scene");
    LoadAsset(test_scene_id);
    
}

void GameUpdate()
{
}

void DrawImGUI()
{
#if defined(NIT_IMGUI_ENABLED) && defined(NIT_EDITOR_ENABLED)

    if (!app->editor.enabled)
    {
        return;
    }
    
    auto& camera_group = GetEntityGroup<Camera, Transform>();
    
    if (!camera_group.entities.empty())
    {
        ImGui::Begin("Camera");
        {
            Entity main_camera = *camera_group.entities.begin();
        
            auto& camera_transform = GetComponent<Transform>(main_camera);
            auto& camera_data      = GetComponent<Camera>(main_camera);
        
            ImGui::DragFloat3("Position", &camera_transform.position.x, .1f);
            ImGui::DragFloat3("Rotation", &camera_transform.rotation.x);
        
            static bool ortho = camera_data.projection == CameraProjection::Orthographic;
            if (ImGui::Checkbox("Otho", &ortho))
            {
                camera_data.projection = ortho ? CameraProjection::Orthographic : CameraProjection::Perspective;
            }
        }
        ImGui::End();

        ImGui::Begin("Entity");
        {
            Entity entity = app->editor.selected_entity;
            
            if (app->editor.is_entity_selected && IsEntityValid(entity))
            {
                auto& camera_transform = GetComponent<Transform>(entity);
        
                ImGui::DragFloat3("Position", &camera_transform.position.x, .1f);
                ImGui::DragFloat3("Rotation", &camera_transform.rotation.x, .1f);
                ImGui::DragFloat3("Scale",    &camera_transform.scale.x, .1f);
            }
            
        }
        ImGui::End();
    }
#endif
}