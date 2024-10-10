#pragma once
#include "nit/core/window.h"
#include "nit/core/system.h"
#include "nit/render/renderer_2d.h"
#include "nit/render/imgui_renderer.h"
#include "nit/logic/entity.h"

#define NIT_CHECK_APP_CREATED NIT_CHECK_MSG(app, "Forget to call SetAppInstance!");

namespace Nit
{
    inline struct App* app = nullptr;

    struct App
    {
        bool stopped        = false;
        bool paused         = false;
        
        TypeRegistry   type_registry;
        AssetRegistry  asset_registry;
        SystemStack    system_stack;
        EntityRegistry entity_registry;
        Renderer2D     renderer_2d;
        Window         window;

#ifdef NIT_IMGUI_ENABLED
        ImGuiRenderer  im_gui_renderer;
#endif
        
        f32 delta_seconds    = 0;
        f64 seconds          = 0;
        u32 frame_count      = 0;
        f64 acc_fixed_delta  = 0;
        f64 last_time        = 0;

        f64 max_delta_time      = 1.f / 15.f;
        f64 fixed_delta_seconds = 0.0166;
    };
    
    void SetAppInstance(App* app_instance);
    void RunApp(VoidFunc run_callback = nullptr);
    void PlayApp();
    void PauseApp();
    void StopApp();
    
    // System shortcuts
    void InitSystemStack();
    void CreateSystem(const String& name, u32 priority = 0, ExecutionContext context = ExecutionContext::Runtime, bool start_disabled = false);
    void SetSystemCallback(VoidFunc callback, Stage stage, bool try_invoke = false);
    void InvokeSystemCallbacks(Stage stage, bool force_enabled = false);
}
