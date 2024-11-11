#include "engine.h"

#include "audio/audio_clip.h"
#include "render/font.h"
#include "render/texture.h"
#include "logic/components.h"
#include "logic/draw_system.h"
#include "logic/scene.h"
#include "render/render_api.h"

#define NIT_CHECK_ENGINE_CREATED NIT_CHECK_MSG(nit::engine::instance, "Forget to call SetAppInstance!");

namespace nit::engine
{
    Engine* instance = nullptr;
    
    void set_instance(Engine* new_engine_instance)
    {
        NIT_CHECK(new_engine_instance);
        instance = new_engine_instance;
    }

    Engine* get_instance()
    {
        return instance;
    }

    void run(VoidFunc run_callback)
    {
        NIT_CHECK_ENGINE_CREATED
        NIT_LOG_TRACE("Creating application...");
        
        window::set_instance(&instance->window);
        window::init();
        
        InitSystemStack();
        
        SetTypeRegistryInstance(&instance->type_registry);
        init_type_registry();

        SetEntityRegistryInstance(&instance->entity_registry);
        InitEntityRegistry();
        
        audio::set_instance(&instance->audio_registry);
        audio::init();
        
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
        FnDrawSystem::Register();
        
        set_asset_registry_instance(&instance->asset_registry);
        
        register_texture_2d_asset();
        register_font_asset();
        RegisterSceneAsset();
        audio::clip::register_type();
        
        if (run_callback)
        {
            run_callback();
        }
        
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
        instance->last_time = window::get_time();
        
        NIT_LOG_TRACE("Application created!");
        
        InvokeSystemCallbacks(Stage::Start, true);
        
        while(!window::should_close())
        {
            instance->frame_count++;
            const f64 current_time = window::get_time();
            const f64 time_between_frames = current_time - instance->last_time;
            instance->last_time = current_time;
            instance->seconds += time_between_frames;
            instance->delta_seconds = (f32) Clamp(time_between_frames, 0., instance->max_delta_time);
            
            InvokeSystemCallbacks(Stage::Update);

            instance->acc_fixed_delta += instance->delta_seconds;
            while (instance->acc_fixed_delta >= instance->fixed_delta_seconds)
            {
                InvokeSystemCallbacks(Stage::FixedUpdate);
                instance->acc_fixed_delta -= instance->fixed_delta_seconds;
            }

            clear_screen();
            
#ifdef NIT_IMGUI_ENABLED
            begin_im_gui();
#endif
            
#ifdef NIT_EDITOR_ENABLED
             editor::begin_draw();
#endif
            InvokeSystemCallbacks(Stage::Draw);

#ifdef NIT_EDITOR_ENABLED
             editor::end_draw();
#endif
#ifdef NIT_IMGUI_ENABLED
            InvokeSystemCallbacks(Stage::DrawImGUI);
            auto [window_width, window_height] = window::get_size();
            end_im_gui(window_width, window_height);
#endif
            
            window::update();
        }

        InvokeSystemCallbacks(Stage::End, true);
    }
    
    void play()  
    {
        NIT_CHECK_ENGINE_CREATED
        instance->paused = false; 

        if (instance->stopped)
        {
            instance->stopped = false;
            InvokeSystemCallbacks(Stage::Start, true);
        }
    }

    void pause() 
    {
        NIT_CHECK_ENGINE_CREATED
        instance->paused = true;  
    }

    void stop()
    {
        NIT_CHECK_ENGINE_CREATED
        InvokeSystemCallbacks(Stage::End, true);
        instance->stopped = true;
    }
}

namespace nit
{
    // System shortcuts

    void InitSystemStack()
    {
        NIT_CHECK_ENGINE_CREATED
        engine::instance->system_stack = SystemStack();
    }

    void CreateSystem(const String& name, u32 priority, ExecutionContext context, bool start_disabled)
    {
        NIT_CHECK_ENGINE_CREATED
        SystemStack& stack = engine::instance->system_stack;
        CreateSystem(stack.systems, stack.next_system, name, priority, context, start_disabled);
    }
    
    void SetSystemCallback(VoidFunc callback, Stage stage, bool try_invoke)
    {
        NIT_CHECK_ENGINE_CREATED
        SystemStack& stack = engine::instance->system_stack;
        SetSystemCallback(stack.systems[stack.next_system - 1], callback, stage, try_invoke);
    }
    
    void InvokeSystemCallbacks(Stage stage, bool force_enabled)
    {
        NIT_CHECK_ENGINE_CREATED
        SystemStack& stack = engine::instance->system_stack;
        InvokeSystemCallbacks(stack.systems, stack.next_system, engine::instance->paused || engine::instance->stopped, stage, force_enabled);
    }
}