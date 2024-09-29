#include "window.h"
#ifdef NIT_GRAPHICS_API_OPENGL
#include <glad/glad.h>
#endif
#include <glfw/glfw3.h>

#define NIT_CHECK_WINDOW_CREATED NIT_CHECK_MSG(window, "Forget to call SetWindowInstance!");

#define NIT_CHECK_WINDOW_INITIALIZED \
    NIT_CHECK_WINDOW_CREATED \
    NIT_CHECK_MSG(window->handler, "Forget to call InitWindow!");

namespace Nit
{
    Window* window = nullptr;

    void SetWindowInstance(Window* window_instance)
    {
        NIT_CHECK(window_instance);
        window = window_instance;
    }
    
    void APIENTRY OnDebugMessageCallback(GLenum source, GLenum type, u32 id, GLenum severity,
        GLsizei length, const char* message, const void* user_param)
    {
        NIT_LOG_TRACE("[OpenGL] %s ", message);
    }
    
    void InitWindow(const WindowCfg& cfg)
    {
        NIT_LOG_TRACE("Creating Window...");
        
        FinishWindow();

        const bool initialized = glfwInit(); 
        NIT_CHECK_MSG(initialized, "GLFW Window initialisation failed!");

        window->handler = glfwCreateWindow(cfg.width, cfg.height, cfg.title.c_str(), nullptr, nullptr);
        NIT_CHECK_MSG(window->handler, "GLFW Window creation failed!");
        glfwMakeContextCurrent(window->handler);
    
#if defined NIT_GRAPHICS_API_OPENGL && NIT_DEBUG
        const bool gl_context = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
        NIT_CHECK_MSG(gl_context, "Failed to set the OpenGL context!");

        if (cfg.render_api_logs)
        {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(OnDebugMessageCallback, nullptr);
        }
#endif
        SetVSync(cfg.v_sync);
        SetWindowTitle(cfg.title);
        SetCursorMode(cfg.cursor_mode);

        if (cfg.start_maximized)
        {
            glfwMaximizeWindow(window->handler);
        }

        NIT_LOG_TRACE("Window created!");
    }

    void FinishWindow()
    {
        NIT_CHECK_WINDOW_CREATED
        if (window->handler)
        {
            CloseWindow();
            glfwDestroyWindow(window->handler);
            window->handler = nullptr;
            glfwTerminate();
        }
    }

    void CloseWindow()
    {
        NIT_CHECK_WINDOW_INITIALIZED
        glfwSetWindowShouldClose(window->handler, true);
    }

    void UpdateWindow()
    {
        glfwPollEvents();
        glfwSwapBuffers(window->handler);
    }

    void SetWindowTitle(const TString& new_title)
    {
        NIT_CHECK_WINDOW_INITIALIZED
        window->title = new_title;
        glfwSetWindowTitle(window->handler, new_title.c_str());
    }

    void SetVSync(bool enabled)
    {
        NIT_CHECK_WINDOW_INITIALIZED
        window->v_sync = enabled;
        glfwSwapInterval(enabled ? 1 : 0);
    }

    void SetCursorMode(const CursorMode mode)
    {
        NIT_CHECK_WINDOW_INITIALIZED
        
        window->cursor_mode = mode;

        switch (mode)
        {
        case CursorMode::Normal:
            glfwSetInputMode(window->handler, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            return;
        case CursorMode::Disabled:
            glfwSetInputMode(window->handler, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            return;
        case CursorMode::Hidden:
            glfwSetInputMode(window->handler, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
            return;
        case CursorMode::Captured:
            glfwSetInputMode(window->handler, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
        }
    }

    bool WindowShouldClose()
    {
        NIT_CHECK_WINDOW_INITIALIZED
        return glfwWindowShouldClose(window->handler);
    }

    void RetrieveWindowSize(i32* width, i32* height)
    {
        NIT_CHECK_WINDOW_INITIALIZED
        glfwGetWindowSize(window->handler, width, height);
    }

    std::pair<f32, f32> GetWindowSize()
    {
        i32 x, y;
        RetrieveWindowSize(&x, &y);
        return { (f32)x, (f32) y };
    }

    float GetWindowAspect()
    {
        i32 x, y;
        RetrieveWindowSize(&x, &y);
        return (f32)x / (f32)y;
    }
    
    void GetCursorPosition(f64* x, f64* y)
    {
        NIT_CHECK_WINDOW_INITIALIZED
        glfwGetCursorPos(window->handler, x, y);
    }

    std::pair<f32, f32> GetCursorPosition()
    {
        f64 x, y;
        GetCursorPosition(&x, &y);
        return { (f32) x, (f32) y };
    }

    i32 GetMouseButton(i32 button)
    {
        NIT_CHECK_WINDOW_INITIALIZED
        return glfwGetMouseButton(window->handler, button);
    }

    i32 GetKey(i32 key)
    {
        NIT_CHECK_WINDOW_INITIALIZED
        return glfwGetKey(window->handler, key);
    }

    const char* GetJoystickName(i32 jid)
    {
        NIT_CHECK_WINDOW_INITIALIZED
        return glfwGetJoystickName(jid);
    }

    const u8* GetJoystickButtons(i32 jid, i32* count)
    {
        NIT_CHECK_WINDOW_INITIALIZED
        return glfwGetJoystickButtons(jid, count);
    }

    const f32* GetJoystickAxes(i32 jid, i32* count)
    {
        NIT_CHECK_WINDOW_INITIALIZED
        return glfwGetJoystickAxes(jid, count);
    }

    f64 GetTime()
    {
        NIT_CHECK_WINDOW_INITIALIZED
        return glfwGetTime();
    }
}