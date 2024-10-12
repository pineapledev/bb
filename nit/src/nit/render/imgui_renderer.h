#pragma once

#ifdef NIT_IMGUI_ENABLED

namespace Nit
{
    struct ImGuiRenderer
    {
        bool use_dockspace = true;
        bool show_demo_window = false;

        // Internal stuff
        const char* dockspace_window_name = "Evi DockSpace";
        const char* dockspace_id = "Evi DockSpace";
        bool is_dockspace_enabled = false;
    };

    void SetImGuiRendererInstance(ImGuiRenderer* im_gui_renderer_instance);
    void InitImGui(void* window_handler);
    void BeginImGui();
    void EndImGui(float window_width, float window_height);
}

#endif