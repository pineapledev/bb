#include "engine.h"

#include "audio/audio_clip.h"
#include "render/font.h"
#include "render/texture.h"
#include "logic/components.h"
#include "logic/draw_system.h"
#include "logic/scene.h"
#include "render/render_api.h"

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
        return engine;
    }

    f32 delta_seconds()
    {
        NIT_CHECK_ENGINE_CREATED
        return engine->delta_seconds;
    }

    EngineEvent& engine_event(Stage stage)
    {
        NIT_CHECK_ENGINE_CREATED
        return engine->events[(u8) stage];
    }

    void engine_run()
    {
        NIT_CHECK_ENGINE_CREATED
        NIT_LOG_TRACE("Creating application...");
        
        window_set_instance(&engine->window);
        window_init();
        
        type_registry_set_instance(&engine->type_registry);
        type_registry_init();

        entity_registry_set_instance(&engine->entity_registry);
        entity_registry_init();
        
        audio_set_instance(&engine->audio_registry);
        audio_init();
        
        register_name_component();
        register_uuid_component();
        register_transform_component();
        register_camera_component();
        register_text_component();
        register_sprite_component();
        register_circle_component();
        register_line_2d_component();

        NIT_IF_EDITOR_ENABLED(register_editor());
        
        register_draw_system();
        
        asset_set_instance(&engine->asset_registry);
        
        register_texture_2d_asset();
        register_font_asset();
        register_scene_asset();
        register_clip_asset();

        event_broadcast(engine_event(Stage::Run));
        
        assets_init();

        set_render_objects_instance(&engine->render_objects);
        init_render_objects();
        
        set_renderer_2d_instance(&engine->renderer_2d);
        init_renderer_2d();

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
            engine->delta_seconds = (f32) Clamp(time_between_frames, 0., engine->max_delta_time);

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

        event_broadcast(engine->events[(u8)Stage::End]);
    }
}