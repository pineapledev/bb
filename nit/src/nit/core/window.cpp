#include "window.h"
#ifdef NIT_GRAPHICS_API_OPENGL
#include <glad/glad.h>
#endif
#include <glfw/glfw3.h>

#define NIT_CHECK_WINDOW_CREATED NIT_CHECK_MSG(window, "Forget to call SetInstance!");

#define NIT_CHECK_WINDOW_INITIALIZED \
    NIT_CHECK_WINDOW_CREATED \
    NIT_CHECK_MSG(window->handler, "Forget to call Init!");

Nit::Window* window = nullptr;

namespace Nit::WindowProc
{
    void SetInstance(Window* window_instance)
    {
        NIT_CHECK(window_instance);
        window = window_instance;
    }
    
    void APIENTRY OnDebugMessageCallback(GLenum source, GLenum type, u32 id, GLenum severity,
        GLsizei length, const char* message, const void* user_param)
    {
        NIT_LOG_TRACE("[OpenGL] %s ", message);
    }
    
    void Init(const WindowCfg& cfg)
    {
        NIT_LOG_TRACE("Creating Window...");
        NIT_CHECK_WINDOW_CREATED
        
        Finish();

        const bool initialized = glfwInit();

        if (!initialized)
        {
            NIT_CHECK_MSG(false, "GLFW Window initialisation failed!");
            return;
        }

        window->handler = glfwCreateWindow(cfg.width, cfg.height, cfg.title.c_str(), nullptr, nullptr);

        if (!window->handler)
        {
            NIT_CHECK_MSG(false, "GLFW Window creation failed!");
            return;
        }
        
        glfwMakeContextCurrent(window->handler);

        const bool gl_context = gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

        if (!gl_context)
        {
            NIT_CHECK_MSG(false, "Failed to set the OpenGL context!");
            return;
        }

#ifdef NIT_GRAPHICS_API_OPENGL
        if (cfg.render_api_logs)
        {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(OnDebugMessageCallback, nullptr);
        }
#endif
        
        SetVSync(cfg.v_sync);
        SetTitle(cfg.title);
        SetCursorMode(cfg.cursor_mode);

        if (cfg.start_maximized)
        {
            glfwMaximizeWindow(window->handler);
        }

        NIT_LOG_TRACE("Window created!");
    }

    void Finish()
    {
        NIT_CHECK_WINDOW_CREATED
        if (window->handler)
        {
            Close();
            glfwDestroyWindow(window->handler);
            window->handler = nullptr;
            glfwTerminate();
        }
    }

    void Close()
    {
        NIT_CHECK_WINDOW_INITIALIZED
        glfwSetWindowShouldClose(window->handler, true);
    }

    void Update()
    {
        glfwPollEvents();
        glfwSwapBuffers(window->handler);
    }

    void SetTitle(const String& new_title)
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

    bool ShouldClose()
    {
        NIT_CHECK_WINDOW_INITIALIZED
        return glfwWindowShouldClose(window->handler);
    }

    void RetrieveSize(i32* width, i32* height)
    {
        NIT_CHECK_WINDOW_INITIALIZED
        glfwGetWindowSize(window->handler, width, height);
    }

    Vector2 GetSize()
    {
        i32 x, y;
        RetrieveSize(&x, &y);
        return { (f32)x, (f32) y };
    }

    float GetAspect()
    {
        i32 x, y;
        RetrieveSize(&x, &y);
        return (f32)x / (f32)y;
    }
    
    void GetCursorPosition(f64* x, f64* y)
    {
        NIT_CHECK_WINDOW_INITIALIZED
        glfwGetCursorPos(window->handler, x, y);
    }

    Vector2 GetCursorPosition()
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

    bool IsMouseButtonPressed(MouseButton button)
    {
        const auto state = GetMouseButton(button);
        return state == GLFW_PRESS;
    }

    i32 GetKey(i32 key)
    {
        NIT_CHECK_WINDOW_INITIALIZED
        return glfwGetKey(window->handler, key);
    }

    bool IsKeyPressed(Key key)
    {
        const auto state = GetKey(key);
        return state == GLFW_PRESS;
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
