#include "nit.h"

using namespace nit;

ListenerAction on_run();
ListenerAction game_start();
ListenerAction game_update();

int main(int argc, char** argv)
{
    Engine engine_instance;
    engine_set_instance(&engine_instance);
    engine_event(Stage::Run) += EngineListener::create(on_run);
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
    entity_add<Transform>(entity).position = position;
    //add<Transform>(entity).position = V3_ZERO;
    entity_add<Sprite>(entity).tint = GetRandomColor();
    //get<Sprite>(entity).texture = test_texture;
    SetSpriteSubTexture2D(entity_get<Sprite>(entity), "cpp");
    reset_movement(entity_get<Transform>(entity), entity_add<Move>(entity));
}

// -----------------------------------------------------------------

ListenerAction on_run()
{
    //Create game system
    engine_event(Stage::Start)  += EngineListener::create(game_start);
    engine_event(Stage::Update) += EngineListener::create(game_update);

    RegisterComponentType<Move>();
    entity_create_group<Transform, Sprite, Move>();

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
    spawn_entity();
    
    for (Entity entity : entity_get_group<Transform, Sprite, Move>().entities)
    {
        auto& transform = entity_get<Transform>(entity);
        auto& move      = entity_get<Move>(entity);

        if (Distance(ToVector2(transform.position), move.destination) < 0.1f)
        {
            reset_movement(transform, move);
        }
        else
        {
            transform.position += ToVector3(move.velocity * delta_seconds());
        }
    }

    return ListenerAction::StayListening;
}

/*
#include "nit.h"

int main(int argc, char** argv)
{
    using namespace nit;

    Window         window;
    RenderObjects  render_objects;
    Renderer2D     renderer_2d;
    TypeRegistry   type_registry;
    
    type_registry_set_instance(&type_registry);
    type_registry_init();
    
    window_set_instance(&window);
    window_init({ .start_maximized = false });

    render_objects_set_instance(&render_objects);
    render_objects_init();
        
    renderer_2d_set_instance(&renderer_2d);
    renderer_2d_init();

    Camera camera;
    camera.projection = CameraProjection::Orthographic;
    
    Transform camera_transform;
    camera_transform.position = { 0, 0, 3 };
    
    Vector3 quad_pos;
    
    while (window_should_close() == false)
    {
        clear_screen();
        
        Vector3 right; // 1, 0, 0
        right.x = 1; 
        quad_pos = quad_pos + right * 0.0001f;

        // position, rotation, scale

        Matrix4 camera_matrix = camera_proj_view(camera, camera_transform);
        
        begin_scene_2d(camera_matrix);
        {
            draw_quad(quad_pos, V3_ZERO, V3_ONE, V4_COLOR_ORANGE);
            draw_quad(V3_ZERO);
        }
        end_scene_2d();
        
        window_update();
    }
}
 */