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
    
    void SetInstance(Engine* engine_instance)
    {
        NIT_CHECK(engine_instance);
        engine = engine_instance;
    }

    Engine* GetInstance()
    {
        return engine;
    }

    void Run(VoidFunc run_callback)
    {
        NIT_CHECK_ENGINE_CREATED
        NIT_LOG_TRACE("Creating application...");
        
        window::SetInstance(&engine::GetInstance()->window);
        window::Init();
        
        InitSystemStack();
        
        SetTypeRegistryInstance(&engine::GetInstance()->type_registry);
        InitTypeRegistry();

        SetEntityRegistryInstance(&engine::GetInstance()->entity_registry);
        InitEntityRegistry();
        
        audio::SetInstance(&engine::GetInstance()->audio_registry);
        audio::Init();
        
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
        
        SetAssetRegistryInstance(&engine::GetInstance()->asset_registry);
        
        RegisterTexture2DAsset();
        RegisterFontAsset();
        RegisterSceneAsset();
        audio::clip::Register();
        
        if (run_callback)
        {
            run_callback();
        }
        
        InitAssetRegistry();

        SetRenderObjectsInstance(&engine::GetInstance()->render_objects);
        InitRenderObjects();
        
        SetRenderer2DInstance(&engine::GetInstance()->renderer_2d);
        InitRenderer2D();

#ifdef NIT_IMGUI_ENABLED
        SetImGuiRendererInstance(&engine::GetInstance()->im_gui_renderer);
        InitImGui(engine::GetInstance()->window.handler);
#endif

#ifdef NIT_EDITOR_ENABLED
        editor::SetInstance(&engine::GetInstance()->editor);
        editor::Init();
#endif
        
        // Init time
        engine::GetInstance()->seconds          = 0;
        engine::GetInstance()->frame_count      = 0;
        engine::GetInstance()->acc_fixed_delta  = 0;
        engine::GetInstance()->last_time = window::GetTime();
        
        NIT_LOG_TRACE("Application created!");
        
        InvokeSystemCallbacks(Stage::Start, true);
        
        while(!window::ShouldClose())
        {
            engine::GetInstance()->frame_count++;
            const f64 current_time = window::GetTime();
            const f64 time_between_frames = current_time - engine::GetInstance()->last_time;
            engine::GetInstance()->last_time = current_time;
            engine::GetInstance()->seconds += time_between_frames;
            engine::GetInstance()->delta_seconds = (f32) Clamp(time_between_frames, 0., engine::GetInstance()->max_delta_time);
            
            InvokeSystemCallbacks(Stage::Update);

            engine::GetInstance()->acc_fixed_delta += engine::GetInstance()->delta_seconds;
            while (engine::GetInstance()->acc_fixed_delta >= engine::GetInstance()->fixed_delta_seconds)
            {
                InvokeSystemCallbacks(Stage::FixedUpdate);
                engine::GetInstance()->acc_fixed_delta -= engine::GetInstance()->fixed_delta_seconds;
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
        engine::GetInstance()->paused = false; 

        if (engine::GetInstance()->stopped)
        {
            engine::GetInstance()->stopped = false;
            InvokeSystemCallbacks(Stage::Start, true);
        }
    }

    void Pause() 
    {
        NIT_CHECK_ENGINE_CREATED
        engine::GetInstance()->paused = true;  
    }

    void Stop()
    {
        NIT_CHECK_ENGINE_CREATED
        InvokeSystemCallbacks(Stage::End, true);
        engine::GetInstance()->stopped = true;
    }
}

namespace nit
{
    // System shortcuts

    void InitSystemStack()
    {
        NIT_CHECK_ENGINE_CREATED
        engine::GetInstance()->system_stack = SystemStack();
    }

    void CreateSystem(const String& name, u32 priority, ExecutionContext context, bool start_disabled)
    {
        NIT_CHECK_ENGINE_CREATED
        SystemStack& stack = engine::GetInstance()->system_stack;
        CreateSystem(stack.systems, stack.next_system, name, priority, context, start_disabled);
    }
    
    void SetSystemCallback(VoidFunc callback, Stage stage, bool try_invoke)
    {
        NIT_CHECK_ENGINE_CREATED
        SystemStack& stack = engine::GetInstance()->system_stack;
        SetSystemCallback(stack.systems[stack.next_system - 1], callback, stage, try_invoke);
    }
    
    void InvokeSystemCallbacks(Stage stage, bool force_enabled)
    {
        NIT_CHECK_ENGINE_CREATED
        SystemStack& stack = engine::GetInstance()->system_stack;
        InvokeSystemCallbacks(stack.systems, stack.next_system, engine::GetInstance()->paused || engine::GetInstance()->stopped, stage, force_enabled);
    }
}