#include "imgui_renderer.h"


#ifdef NIT_IMGUI_ENABLED

#include <ImGuizmo.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

#define NIT_CHECK_IM_GUI_CREATED NIT_CHECK_MSG(im_gui_renderer, "ImGuiRenderer is not created!");

namespace nit
{
    ImGuiRenderer* im_gui_renderer = nullptr;

    void set_im_gui_renderer_instance(ImGuiRenderer* im_gui_renderer_instance)
    {
        NIT_CHECK(im_gui_renderer_instance);
        im_gui_renderer = im_gui_renderer_instance;
    }

    void init_im_gui(void* window_handler)
    {
        NIT_CHECK_IM_GUI_CREATED
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

        NIT_CHECK(window_handler);
        ImGui_ImplGlfw_InitForOpenGL(static_cast<GLFWwindow*>(window_handler), true);
        ImGui_ImplOpenGL3_Init("#version 410");

        ImGuiStyle& style = ImGui::GetStyle();
        style.Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    }

    void begin_im_gui()
    {
        NIT_CHECK_IM_GUI_CREATED
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuizmo::BeginFrame();

        if (im_gui_renderer->is_dockspace_enabled)
        {
            ImGuiDockNodeFlags dock_space_flags = ImGuiDockNodeFlags_None;
            ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

            const ImGuiViewport* viewport = ImGui::GetMainViewport();

            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            
            window_flags |= ImGuiWindowFlags_NoTitleBar;
            window_flags |= ImGuiWindowFlags_NoCollapse;
            window_flags |= ImGuiWindowFlags_NoResize;
            window_flags |= ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
            window_flags |= ImGuiWindowFlags_NoNavFocus;
            window_flags |= ImGuiWindowFlags_NoBackground;

            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

            ImGui::Begin(im_gui_renderer->dockspace_window_name, nullptr, window_flags);
            ImGui::PopStyleVar();
            ImGui::PopStyleVar(2);
        
            // Submit the DockSpace
            ImGuiIO& io = ImGui::GetIO();

            if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
            {
                ImGui::DockSpace(ImGui::GetID(im_gui_renderer->dockspace_id), ImVec2(0.0f, 0.0f), dock_space_flags);
            }
        }

        if (im_gui_renderer->show_demo_window)
        {
            ImGui::ShowDemoWindow();
        }
    }

    void end_im_gui(float window_width, float window_height)
    {
        NIT_CHECK_IM_GUI_CREATED
        
        if (im_gui_renderer->is_dockspace_enabled)
        {
            ImGui::End();
        }
        
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = { window_width, window_height };

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        if (im_gui_renderer->is_dockspace_enabled != im_gui_renderer->use_dockspace)
        {
            im_gui_renderer->is_dockspace_enabled = im_gui_renderer->use_dockspace;
        }
    }
}

#endif