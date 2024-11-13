#pragma once
#include "nit/core/window.h"
#include "nit/core/asset.h"
#include "nit/render/renderer_2d.h"
#include "nit/render/imgui_renderer.h"
#include "nit/logic/entity.h"
#include "nit/render/render_objects.h"
#include "nit/audio/audio.h"

#ifdef NIT_EDITOR_ENABLED
#include "nit/editor/editor.h"
#endif

namespace nit
{
    enum class ExecutionContext : u8
    {
        Runtime
      , Editor
      , Always
    };
    
    enum class Stage : u8
    {
        Run
      , Start
      , Update
      , FixedUpdate
      , Draw
      , DrawImGUI
      , End
      , Count
    };

    using EngineEvent    = Event<>;
    using EngineListener = Listener<>;
    
    struct Engine
    {
        bool stopped        = false;
        bool paused         = false;
        
        EngineEvent    events[(u8) Stage::Count];
        
        TypeRegistry   type_registry;
        AssetRegistry  asset_registry;
        EntityRegistry entity_registry;
        RenderObjects  render_objects;
        Renderer2D     renderer_2d;
        AudioRegistry  audio_registry;
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
    
    void         engine_set_instance(Engine* new_engine_instance);
    Engine*      engine_get_instance();
    EngineEvent& engine_event(Stage stage);
    void         engine_run();
    void         engine_play();
    void         engine_pause();
    void         engine_stop();
}
