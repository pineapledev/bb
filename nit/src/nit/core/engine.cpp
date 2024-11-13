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
        
        SetTypeRegistryInstance(&instance->type_registry);
        init_type_registry();

        entity::set_registry_instance(&instance->entity_registry);
        InitEntityRegistry();
        
        audio::audio_set_instance(&instance->audio_registry);
        audio::audio_init();
        
        RegisterNameComponent();
        RegisterUUIDComponent();
        RegisterTransformComponent();
        RegisterCameraComponent();
        RegisterTextComponent();
        RegisterSpriteComponent();
        RegisterCircleComponent();
        RegisterLine2DComponent();

#ifdef NIT_EDITOR_ENABLED
        editor::type_register();  
#endif
        draw_system::type_register();
        
        set_asset_registry_instance(&instance->asset_registry);
        
        register_texture_2d_asset();
        register_font_asset();
        RegisterSceneAsset();
        audio::clip::type_register();

        broadcast(instance->events[(u8)Stage::Run]);
        
        init_asset_registry();

        set_render_objects_instance(&instance->render_objects);
        init_render_objects();
        
        set_renderer_2d_instance(&instance->renderer_2d);
        init_renderer_2d();

#ifdef NIT_IMGUI_ENABLED
        set_im_gui_renderer_instance(&instance->im_gui_renderer);
        init_im_gui(instance->window.handler);
#endif

#ifdef NIT_EDITOR_ENABLED
        editor::set_instance(&instance->editor);
        editor::init();
#endif
        
        // Init time
        instance->seconds          = 0;
        instance->frame_count      = 0;
        instance->acc_fixed_delta  = 0;
        instance->last_time = window_get_time();
        
        NIT_LOG_TRACE("Application created!");
        
        broadcast(instance->events[(u8)Stage::Start]);
        
        while(!window_should_close())
        {
            instance->frame_count++;
            const f64 current_time = window_get_time();
            const f64 time_between_frames = current_time - instance->last_time;
            instance->last_time = current_time;
            instance->seconds += time_between_frames;
            instance->delta_seconds = (f32) Clamp(time_between_frames, 0., instance->max_delta_time);
            
            broadcast(instance->events[(u8)Stage::Update]);

            instance->acc_fixed_delta += instance->delta_seconds;
            while (instance->acc_fixed_delta >= instance->fixed_delta_seconds)
            {
                broadcast(instance->events[(u8)Stage::FixedUpdate]);
                instance->acc_fixed_delta -= instance->fixed_delta_seconds;
            }

            clear_screen();
            
#ifdef NIT_IMGUI_ENABLED
            begin_im_gui();
#endif
            
#ifdef NIT_EDITOR_ENABLED
             editor::begin_draw();
#endif
            broadcast(instance->events[(u8)Stage::Draw]);

#ifdef NIT_EDITOR_ENABLED
             editor::end_draw();
#endif
#ifdef NIT_IMGUI_ENABLED
            broadcast(instance->events[(u8)Stage::DrawImGUI]);
            auto [window_width, window_height] = window_get_size();
            end_im_gui(window_width, window_height);
#endif
            
            window_update();
        }

        broadcast(instance->events[(u8)Stage::End]);
    }

    void engine_play()  
    {
        NIT_CHECK_ENGINE_CREATED
        instance->paused = false; 

        if (instance->stopped)
        {
            instance->stopped = false;
            broadcast(instance->events[(u8)Stage::Start]);
        }
    }

    void engine_pause() 
    {
        NIT_CHECK_ENGINE_CREATED
        instance->paused = true;  
    }

    void engine_stop()
    {
        NIT_CHECK_ENGINE_CREATED
        broadcast(instance->events[(u8)Stage::End]);
        instance->stopped = true;
    }
}