#include "engine.h"

#include "audio/audio_clip.h"
#include "render/font.h"
#include "render/texture.h"
#include "logic/components.h"
#include "logic/scene.h"
#include "logic/systems.h"
#include "render/render_api.h"

#define NIT_CHECK_ENGINE_CREATED NIT_CHECK_MSG(engine, "Forget to call SetAppInstance!");

namespace Nit::FnEngine
{
    void SetInstance(Engine* engine_instance)
    {
        NIT_CHECK(engine_instance);
        engine = engine_instance;
    }
    
    void Run(VoidFunc run_callback)
    {
        NIT_CHECK_ENGINE_CREATED
        NIT_LOG_TRACE("Creating application...");
        
        FnWindow::SetInstance(&engine->window);
        FnWindow::Init();
        
        InitSystemStack();
        
        SetTypeRegistryInstance(&engine->type_registry);
        InitTypeRegistry();

        SetEntityRegistryInstance(&engine->entity_registry);
        InitEntityRegistry();
        
        FnAudioRegistry::SetInstance(&engine->audio_registry);
        FnAudioRegistry::Init();
        
        RegisterNameComponent();
        RegisterUUIDComponent();
        RegisterTransformComponent();
        RegisterCameraComponent();
        RegisterTextComponent();
        RegisterSpriteComponent();
        RegisterCircleComponent();
        RegisterLine2DComponent();

        CreateDrawSystem();
        
        SetAssetRegistryInstance(&engine->asset_registry);
        
        RegisterTexture2DAsset();
        RegisterFontAsset();
        RegisterSceneAsset();
        FnAudioClip::Register();
        
        if (run_callback)
        {
            run_callback();
        }
        
        InitAssetRegistry();

        SetRenderObjectsInstance(&engine->render_objects);
        InitRenderObjects();
        
        SetRenderer2DInstance(&engine->renderer_2d);
        InitRenderer2D();

#ifdef NIT_IMGUI_ENABLED
        SetImGuiRendererInstance(&engine->im_gui_renderer);
        InitImGui(engine->window.handler);
#endif

#ifdef NIT_EDITOR_ENABLED
        SetEditorInstance(&engine->editor);
        InitEditor();
#endif
        
        // Init time
        engine->seconds          = 0;
        engine->frame_count      = 0;
        engine->acc_fixed_delta  = 0;
        engine->last_time = FnWindow::GetTime();
        
        NIT_LOG_TRACE("Application created!");
        
        InvokeSystemCallbacks(Stage::Start, true);
        
        while(!FnWindow::ShouldClose())
        {
            engine->frame_count++;
            const f64 current_time = FnWindow::GetTime();
            const f64 time_between_frames = current_time - engine->last_time;
            engine->last_time = current_time;
            engine->seconds += time_between_frames;
            engine->delta_seconds = (f32) Clamp(time_between_frames, 0., engine->max_delta_time);
            
            InvokeSystemCallbacks(Stage::Update);

            engine->acc_fixed_delta += engine->delta_seconds;
            while (engine->acc_fixed_delta >= engine->fixed_delta_seconds)
            {
                InvokeSystemCallbacks(Stage::FixedUpdate);
                engine->acc_fixed_delta -= engine->fixed_delta_seconds;
            }

            ClearScreen();
            
#ifdef NIT_IMGUI_ENABLED
            BeginImGui();
#endif
            
#ifdef NIT_EDITOR_ENABLED
            BeginDrawEditor();
#endif
            InvokeSystemCallbacks(Stage::Draw);

#ifdef NIT_EDITOR_ENABLED
            EndDrawEditor();
#endif
#ifdef NIT_IMGUI_ENABLED
            InvokeSystemCallbacks(Stage::DrawImGUI);
            auto [window_width, window_height] = FnWindow::GetSize();
            EndImGui(window_width, window_height);
#endif
            
            FnWindow::Update();
        }

        InvokeSystemCallbacks(Stage::End, true);
    }
    
    void Play()  
    {
        NIT_CHECK_ENGINE_CREATED
        engine->paused = false; 

        if (engine->stopped)
        {
            engine->stopped = false;
            InvokeSystemCallbacks(Stage::Start, true);
        }
    }

    void Pause() 
    {
        NIT_CHECK_ENGINE_CREATED
        engine->paused = true;  
    }

    void Stop()
    {
        NIT_CHECK_ENGINE_CREATED
        InvokeSystemCallbacks(Stage::End, true);
        engine->stopped = true;
    }
}

namespace Nit
{
    // System shortcuts

    void InitSystemStack()
    {
        NIT_CHECK_ENGINE_CREATED
        engine->system_stack = SystemStack();
    }

    void CreateSystem(const String& name, u32 priority, ExecutionContext context, bool start_disabled)
    {
        NIT_CHECK_ENGINE_CREATED
        SystemStack& stack = engine->system_stack;
        CreateSystem(stack.systems, stack.next_system, name, priority, context, start_disabled);
    }
    
    void SetSystemCallback(VoidFunc callback, Stage stage, bool try_invoke)
    {
        NIT_CHECK_ENGINE_CREATED
        SystemStack& stack = engine->system_stack;
        SetSystemCallback(stack.systems[stack.next_system - 1], callback, stage, try_invoke);
    }
    
    void InvokeSystemCallbacks(Stage stage, bool force_enabled)
    {
        NIT_CHECK_ENGINE_CREATED
        SystemStack& stack = engine->system_stack;
        InvokeSystemCallbacks(stack.systems, stack.next_system, engine->paused || engine->stopped, stage, force_enabled);
    }
}