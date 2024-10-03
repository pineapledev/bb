#include "app.h"

namespace Nit
{
    void SetAppInstance(App* app_instance)
    {
        NIT_CHECK(app_instance);
        app = app_instance;
    }
    
    void RunApp(VoidFunc run_callback)
    {
        NIT_CHECK_APP_CREATED
        NIT_LOG_TRACE("Creating application...");

        SetTypeRegistryInstance(&app->type_registry);
        InitTypeRegistry();
        
        SetWindowInstance(&app->window);
        InitWindow();
        
        SetRenderer2DInstance(&app->renderer_2d);
        InitRenderer2D();
        
        SetImGuiRendererInstance(&app->im_gui_renderer);
        InitImGui(app->window.handler);
        
        InitEntityRegistry(app->entity_registry);

        InitSystemStack();

        // Init time
        app->seconds          = 0;
        app->frame_count      = 0;
        app->acc_fixed_delta  = 0;
        app->last_time = GetTime();
        
        NIT_LOG_TRACE("Application created!");
        
        if (run_callback)
        {
            run_callback();
        }
        
        InvokeSystemCallbacks(Stage::Start, true);
        
        while(!WindowShouldClose())
        {
            app->frame_count++;
            const f64 current_time = GetTime();
            const f64 time_between_frames = current_time - app->last_time;
            app->last_time = current_time;
            app->seconds += time_between_frames;
            app->delta_seconds = (f32) Clamp(time_between_frames, 0., app->max_delta_time);
            
            InvokeSystemCallbacks(Stage::Update);

            app->acc_fixed_delta += app->delta_seconds;
            while (app->acc_fixed_delta >= app->fixed_delta_seconds)
            {
                InvokeSystemCallbacks(Stage::FixedUpdate);
                app->acc_fixed_delta -= app->fixed_delta_seconds;
            }
            
            InvokeSystemCallbacks(Stage::Draw);

            if (app->im_gui_enabled)
            {
                BeginImGui();
                InvokeSystemCallbacks(Stage::DrawImGUI);
                auto [window_width, window_height] = GetWindowSize(); 
                EndImGui(window_width, window_height);
            }
            
            UpdateWindow();
        }

        InvokeSystemCallbacks(Stage::End, true);
    }

    void PlayApp()  
    {
        NIT_CHECK_APP_CREATED
        app->paused = false; 

        if (app->stopped)
        {
            app->stopped = false;
            InvokeSystemCallbacks(Stage::Start, true);
        }
    }

    void PauseApp() 
    {
        NIT_CHECK_APP_CREATED
        app->paused = true;  
    }

    void StopApp()
    {
        NIT_CHECK_APP_CREATED
        InvokeSystemCallbacks(Stage::End, true);
        app->stopped = true;
    }

    // System shortcuts

    void InitSystemStack()
    {
        NIT_CHECK_APP_CREATED
        app->system_stack = SystemStack();
    }

    void CreateSystem(const String& name, u32 priority, ExecutionContext context, bool start_disabled)
    {
        NIT_CHECK_APP_CREATED
        SystemStack& stack = app->system_stack;
        CreateSystem(stack.systems, stack.next_system, name, priority, context, start_disabled);
    }
    
    void SetSystemCallback(VoidFunc callback, Stage stage, bool try_invoke)
    {
        NIT_CHECK_APP_CREATED
        SystemStack& stack = app->system_stack;
        SetSystemCallback(stack.systems[stack.next_system - 1], callback, stage, try_invoke);
    }
    
    void InvokeSystemCallbacks(Stage stage, bool force_enabled)
    {
        NIT_CHECK_APP_CREATED
        SystemStack& stack = app->system_stack;
        InvokeSystemCallbacks(stack.systems, stack.next_system, app->paused || app->stopped, stage, force_enabled);
    }

    // Entity shortcuts
    
    Entity CreateEntity()
    {
        NIT_CHECK_APP_CREATED
        return CreateEntity(app->entity_registry);
    }

    void DestroyEntity(Entity entity)
    {
        NIT_CHECK_APP_CREATED
        DestroyEntity(app->entity_registry, entity);
    }

    bool IsEntityValid(Entity entity)
    {
        NIT_CHECK_APP_CREATED
        return IsEntityValid(app->entity_registry, entity);
    }

    EntityGroup& GetEntityGroup(EntitySignature signature)
    {
        NIT_CHECK_APP_CREATED
        return GetEntityGroup(app->entity_registry, signature);
    }
}