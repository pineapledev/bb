#include "engine.h"

#include "audio/audio_clip.h"
#include "render/font.h"
#include "render/texture.h"
#include "logic/components.h"
#include "logic/draw_system.h"
#include "logic/scene.h"
#include "render/render_api.h"

#define NIT_CHECK_ENGINE_CREATED NIT_CHECK_MSG(engine::engine, "Forget to call SetAppInstance!");

namespace nit::engine
{
    Engine* engine = nullptr;
    
    void set_instance(Engine* engine_instance)
    {
        NIT_CHECK(engine_instance);
        engine = engine_instance;
    }

    Engine* get_instance()
    {
        return engine;
    }

    void Run(VoidFunc run_callback)
    {
        NIT_CHECK_ENGINE_CREATED
        NIT_LOG_TRACE("Creating application...");
        
        window::set_instance(&engine::get_instance()->window);
        window::Init();
        
        InitSystemStack();
        
        SetTypeRegistryInstance(&engine::get_instance()->type_registry);
        InitTypeRegistry();

        SetEntityRegistryInstance(&engine::get_instance()->entity_registry);
        InitEntityRegistry();
        
        audio::set_instance(&engine::get_instance()->audio_registry);
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
        editor::Register();  
#endif
        FnDrawSystem::Register();
        
        set_asset_registry_instance(&engine::get_instance()->asset_registry);
        
        RegisterTexture2DAsset();
        RegisterFontAsset();
        RegisterSceneAsset();
        audio::clip::register_type();
        
        if (run_callback)
        {
            run_callback();
        }
        
        init_asset_registry();

        SetRenderObjectsInstance(&engine::get_instance()->render_objects);
        InitRenderObjects();
        
        SetRenderer2DInstance(&engine::get_instance()->renderer_2d);
        InitRenderer2D();

#ifdef NIT_IMGUI_ENABLED
        SetImGuiRendererInstance(&engine::get_instance()->im_gui_renderer);
        InitImGui(engine::get_instance()->window.handler);
#endif

#ifdef NIT_EDITOR_ENABLED
        editor::set_instance(&engine::get_instance()->editor);
        editor::Init();
#endif
        
        // Init time
        engine::get_instance()->seconds          = 0;
        engine::get_instance()->frame_count      = 0;
        engine::get_instance()->acc_fixed_delta  = 0;
        engine::get_instance()->last_time = window::GetTime();
        
        NIT_LOG_TRACE("Application created!");
        
        InvokeSystemCallbacks(Stage::Start, true);
        
        while(!window::ShouldClose())
        {
            engine::get_instance()->frame_count++;
            const f64 current_time = window::GetTime();
            const f64 time_between_frames = current_time - engine::get_instance()->last_time;
            engine::get_instance()->last_time = current_time;
            engine::get_instance()->seconds += time_between_frames;
            engine::get_instance()->delta_seconds = (f32) Clamp(time_between_frames, 0., engine::get_instance()->max_delta_time);
            
            InvokeSystemCallbacks(Stage::Update);

            engine::get_instance()->acc_fixed_delta += engine::get_instance()->delta_seconds;
            while (engine::get_instance()->acc_fixed_delta >= engine::get_instance()->fixed_delta_seconds)
            {
                InvokeSystemCallbacks(Stage::FixedUpdate);
                engine::get_instance()->acc_fixed_delta -= engine::get_instance()->fixed_delta_seconds;
            }

            ClearScreen();
            
#ifdef NIT_IMGUI_ENABLED
            BeginImGui();
#endif
            
#ifdef NIT_EDITOR_ENABLED
             editor::BeginDraw();
#endif
            InvokeSystemCallbacks(Stage::Draw);

#ifdef NIT_EDITOR_ENABLED
             editor::EndDraw();
#endif
#ifdef NIT_IMGUI_ENABLED
            InvokeSystemCallbacks(Stage::DrawImGUI);
            auto [window_width, window_height] = window::GetSize();
            EndImGui(window_width, window_height);
#endif
            
            window::Update();
        }

        InvokeSystemCallbacks(Stage::End, true);
    }
    
    void Play()  
    {
        NIT_CHECK_ENGINE_CREATED
        engine::get_instance()->paused = false; 

        if (engine::get_instance()->stopped)
        {
            engine::get_instance()->stopped = false;
            InvokeSystemCallbacks(Stage::Start, true);
        }
    }

    void Pause() 
    {
        NIT_CHECK_ENGINE_CREATED
        engine::get_instance()->paused = true;  
    }

    void Stop()
    {
        NIT_CHECK_ENGINE_CREATED
        InvokeSystemCallbacks(Stage::End, true);
        engine::get_instance()->stopped = true;
    }
}

namespace nit
{
    // System shortcuts

    void InitSystemStack()
    {
        NIT_CHECK_ENGINE_CREATED
        engine::get_instance()->system_stack = SystemStack();
    }

    void CreateSystem(const String& name, u32 priority, ExecutionContext context, bool start_disabled)
    {
        NIT_CHECK_ENGINE_CREATED
        SystemStack& stack = engine::get_instance()->system_stack;
        CreateSystem(stack.systems, stack.next_system, name, priority, context, start_disabled);
    }
    
    void SetSystemCallback(VoidFunc callback, Stage stage, bool try_invoke)
    {
        NIT_CHECK_ENGINE_CREATED
        SystemStack& stack = engine::get_instance()->system_stack;
        SetSystemCallback(stack.systems[stack.next_system - 1], callback, stage, try_invoke);
    }
    
    void InvokeSystemCallbacks(Stage stage, bool force_enabled)
    {
        NIT_CHECK_ENGINE_CREATED
        SystemStack& stack = engine::get_instance()->system_stack;
        InvokeSystemCallbacks(stack.systems, stack.next_system, engine::get_instance()->paused || engine::get_instance()->stopped, stage, force_enabled);
    }
}