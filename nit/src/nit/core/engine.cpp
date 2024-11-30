#include "engine.h"

#include "entity/scene.h"

#include "render/draw_system.h"
#include "render/render_api.h"
#include "render/font.h"
#include "render/texture.h"
#include "render/transform.h"
#include "render/camera.h"
#include "render/text.h"
#include "render/sprite.h"
#include "render/line_2d.h"
#include "render/circle.h"

#include "audio/audio_clip.h"
#include "physics/rigidbody_2d.h"
#include "physics/box_collider_2d.h"
#include "physics/circle_collider.h"
#include "physics/physic_material.h"

#define NIT_CHECK_ENGINE_CREATED NIT_CHECK_MSG(nit::engine, "Forget to call SetAppInstance!");

namespace nit
{
    Engine* engine = nullptr;
    
    void engine_set_instance(Engine* new_engine_instance)
    {
        NIT_CHECK(new_engine_instance);
        engine = new_engine_instance;
    }

    Engine* engine_get_instance()
    {
        NIT_CHECK(engine);
        return engine;
    }

    bool engine_has_instance()
    {
        return engine != nullptr;
    }

    static void engine_try_lazy_create()
    {
        if (engine_has_instance())
        {
            return;
        }

        static Engine engine_instance;
        engine_set_instance(&engine_instance);
    }
    
    f32 delta_seconds()
    {
        NIT_CHECK_ENGINE_CREATED
        return engine->delta_seconds;
    }

    f32 fixed_delta_seconds()
    {
        NIT_CHECK_ENGINE_CREATED
        return (f32) engine->fixed_delta_seconds;
    }

    EngineEvent& engine_event(Stage stage)
    {
        engine_try_lazy_create();
        return engine->events[(u8) stage];
    }

    void engine_init(VoidFunc on_init)
    {
        engine_try_lazy_create();
        
        NIT_LOG_TRACE("Creating application...");
        
        window_set_instance(&engine->window);
        window_init();
        
        type_registry_set_instance(&engine->type_registry);
        type_registry_init();

        entity_registry_set_instance(&engine->entity_registry);
        entity_registry_init();
        
        {
            register_name_component();
            register_uuid_component();
            register_transform_component();
            register_camera_component();
            register_text_component();
            register_sprite_component();
            register_circle_component();
            register_line_2d_component();
            register_rigidbody_2d_component();
            register_box_collider_2d_component();
            register_circle_collider_component();
        }

        render_objects_set_instance(&engine->render_objects);
        render_objects_init();
        
        renderer_2d_set_instance(&engine->renderer_2d);
        renderer_2d_init();

        audio_set_instance(&engine->audio_registry);
        audio_init();
        
        physics_2d_set_instance(&engine->physics_2d);
        physics_2d_init();

        NIT_IF_EDITOR_ENABLED(register_editor());
        
        register_draw_system();
        
        asset_registry_set_instance(&engine->asset_registry);
        
        {
            register_texture_2d_asset();
            register_font_asset();
            register_scene_asset();
            register_clip_asset();
            register_physic_material_asset();
        }

        if (on_init)
        {
            on_init();
        }
        
        event_broadcast(engine_event(Stage::Init));
        
        asset_registry_deserialize();

        NIT_IF_EDITOR_ENABLED(im_gui_renderer_set_instance(&engine->im_gui_renderer));
        NIT_IF_EDITOR_ENABLED(im_gui_init(engine->window.handler));

        NIT_IF_EDITOR_ENABLED(editor_set_instance(&engine->editor));
        NIT_IF_EDITOR_ENABLED(editor_init());
        
        // Init time
        engine->seconds          = 0;
        engine->frame_count      = 0;
        engine->acc_fixed_delta  = 0;
        engine->last_time = window_get_time();
        
        NIT_LOG_TRACE("Application created!");
        
        event_broadcast(engine_event(Stage::Start));
        
        while(!window_should_close())
        {
            engine->frame_count++;
            const f64 current_time = window_get_time();
            const f64 time_between_frames = current_time - engine->last_time;
            engine->last_time = current_time;
            engine->seconds += time_between_frames;
            engine->delta_seconds = (f32) clamp(time_between_frames, 0., engine->max_delta_time);

            event_broadcast(engine_event(Stage::Update));

            engine->acc_fixed_delta += engine->delta_seconds;
            
            while (engine->acc_fixed_delta >= engine->fixed_delta_seconds)
            {
                event_broadcast(engine_event(Stage::FixedUpdate));
                engine->acc_fixed_delta -= engine->fixed_delta_seconds;
            }
            
            event_broadcast(engine_event(Stage::LateUpdate));

            clear_screen();
            
            NIT_IF_EDITOR_ENABLED(im_gui_begin());
            NIT_IF_EDITOR_ENABLED(editor_begin());
            
            event_broadcast(engine_event(Stage::PreDraw));
            event_broadcast(engine_event(Stage::Draw));
            event_broadcast(engine_event(Stage::PostDraw));

            NIT_IF_EDITOR_ENABLED(editor_end());
            NIT_IF_EDITOR_ENABLED(im_gui_end(window_get_size()));
            
            window_update();
        }

        event_broadcast(engine_event(Stage::End));
    }
}