#pragma once
#include "nit/core/window.h"
#include "nit/core/system.h"
#include "nit/core/asset.h"
#include "nit/render/renderer_2d.h"
#include "nit/render/imgui_renderer.h"
#include "nit/logic/entity.h"
#include "nit/render/render_objects.h"

#ifdef NIT_EDITOR_ENABLED
#include "nit/editor/editor.h"
#endif

namespace Nit
{
    inline struct Engine* engine = nullptr;

    struct Engine
    {
        bool stopped        = false;
        bool paused         = false;
        
        TypeRegistry   type_registry;
        AssetRegistry  asset_registry;
        SystemStack    system_stack;
        EntityRegistry entity_registry;
        RenderObjects  render_objects;
        Renderer2D     renderer_2d;
        Window         window;

#ifdef NIT_IMGUI_ENABLED
        ImGuiRenderer  im_gui_renderer;
#endif
#ifdef NIT_EDITOR_ENABLED
        Editor editor;
#endif
        
        f32 delta_seconds    = 0;
        f64 seconds          = 0;
        u32 frame_count      = 0;
        f64 acc_fixed_delta  = 0;
        f64 last_time        = 0;

        f64 max_delta_time      = 1.f / 15.f;
        f64 fixed_delta_seconds = 0.0166;
    };
    
    namespace EngineProc
    {
        void SetInstance(Engine* engine_instance);
        void Run(VoidFunc run_callback = nullptr);
        void Play();
        void Pause();
        void Stop();
    }
    
    // System shortcuts
    void InitSystemStack();
    void CreateSystem(const String& name, u32 priority = 0, ExecutionContext context = ExecutionContext::Runtime, bool start_disabled = false);
    void SetSystemCallback(VoidFunc callback, Stage stage, bool try_invoke = false);
    void InvokeSystemCallbacks(Stage stage, bool force_enabled = false);
}
