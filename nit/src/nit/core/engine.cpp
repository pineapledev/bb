#include "engine.h"

#include "audio/audio_clip.h"
#include "render/font.h"
#include "render/texture.h"
#include "logic/components.h"
#include "logic/draw_system.h"
#include "logic/scene.h"
#include "render/render_api.h"

#define NIT_CHECK_ENGINE_CREATED NIT_CHECK_MSG(nit::instance, "Forget to call SetAppInstance!");

namespace nit
{
    Engine* instance = nullptr;
    
    void engine_set_instance(Engine* new_engine_instance)
    {
        NIT_CHECK(new_engine_instance);
        instance = new_engine_instance;
    }

    Engine* engine_get_instance()
    {
        return instance;
    }

    EngineEvent& engine_event(Stage stage)
    {
        NIT_CHECK_ENGINE_CREATED
        return instance->events[(u8) stage];
    }

    void engine_run()
    {
        NIT_CHECK_ENGINE_CREATED
        NIT_LOG_TRACE("Creating application...");
        
        window_set_instance(&instance->window);
        window_init();
        
        type_registry_set_instance(&instance->type_registry);
        type_registry_init();

        entity::set_registry_instance(&instance->entity_registry);
        InitEntityRegistry();
        
        audio_set_instance(&instance->audio_registry);
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
        
        asset_set_instance(&instance->asset_registry);
        
        register_texture_2d_asset();
        register_font_asset();
        register_scene_asset();
        register_clip_asset();

        event_broadcast(engine_event(Stage::Run));
        
        assets_init();

        set_render_objects_instance(&instance->render_objects);
        init_render_objects();
        
        set_renderer_2d_instance(&instance->renderer_2d);
        init_renderer_2d();

        NIT_IF_EDITOR_ENABLED(im_gui_renderer_set_instance(&instance->im_gui_renderer));
        NIT_IF_EDITOR_ENABLED(im_gui_init(instance->window.handler));

        NIT_IF_EDITOR_ENABLED(editor_set_instance(&instance->editor));
        NIT_IF_EDITOR_ENABLED(editor_init());
        
        // Init time
        instance->seconds          = 0;
        instance->frame_count      = 0;
        instance->acc_fixed_delta  = 0;
        instance->last_time = window_get_time();
        
        NIT_LOG_TRACE("Application created!");
        
        event_broadcast(engine_event(Stage::Start));
        
        while(!window_should_close())
        {
            instance->frame_count++;
            const f64 current_time = window_get_time();
            const f64 time_between_frames = current_time - instance->last_time;
            instance->last_time = current_time;
            instance->seconds += time_between_frames;
            instance->delta_seconds = (f32) Clamp(time_between_frames, 0., instance->max_delta_time);

            event_broadcast(engine_event(Stage::Update));

            instance->acc_fixed_delta += instance->delta_seconds;
            
            while (instance->acc_fixed_delta >= instance->fixed_delta_seconds)
            {
                event_broadcast(engine_event(Stage::FixedUpdate));
                instance->acc_fixed_delta -= instance->fixed_delta_seconds;
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

        event_broadcast(instance->events[(u8)Stage::End]);
    }
}