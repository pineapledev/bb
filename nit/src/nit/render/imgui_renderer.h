#pragma once

#ifdef NIT_IMGUI_ENABLED

namespace nit
{
    struct ImGuiRenderer
    {
#ifdef NIT_EDITOR_ENABLED
        bool use_dockspace = true;
        bool is_dockspace_enabled = true;
#else
        bool use_dockspace = false;
        bool is_dockspace_enabled = false;
#endif
        
        bool show_demo_window = false;

        const char* dockspace_window_name = "Evi DockSpace";
        const char* dockspace_id = "Evi DockSpace";
    };

    void im_gui_renderer_set_instance(ImGuiRenderer* im_gui_renderer_instance);
    void im_gui_init(void* window_handler);
    void im_gui_begin();
    void im_gui_end(const Vector2& window_size);
}

#endif