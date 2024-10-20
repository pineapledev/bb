#pragma once
#include "key_codes.h"

struct GLFWwindow;

namespace Nit
{
    enum class CursorMode : u8
    {
        Normal
      , Disabled
      , Hidden
      , Captured
    };
    
    struct WindowCfg
    {
        String     title           = "Window :3";
        i32         width           = 1280;
        i32         height          = 720;
        bool        v_sync          = true;
        bool        start_maximized = false;
        CursorMode  cursor_mode     = CursorMode::Normal;
        bool        render_api_logs = false;
    };

    void                SetWindowInstance(struct Window* window_instance);
    void                InitWindow(const WindowCfg& cfg = {});
    void                FinishWindow();
    void                CloseWindow();
    void                UpdateWindow();
        
    void                SetWindowTitle(const String& new_title);
    void                SetVSync(bool enabled);
    void                SetCursorMode(const CursorMode mode);

    bool                WindowShouldClose();
    void                RetrieveWindowSize(i32* width, i32* height);
    Vector2             GetWindowSize();
    float               GetWindowAspect();
    void                GetCursorPosition(f64* x, f64* y);
    Vector2             GetCursorPosition();
    i32                 GetMouseButton(i32 button);
    bool                IsMouseButtonPressed(MouseButton button);
    i32                 GetKey(i32 key);
    bool                IsKeyPressed(Key key);
    const char*         GetJoystickName(i32 jid);
    const u8*           GetJoystickButtons(i32 jid, i32* count);
    const f32*          GetJoystickAxes(i32 jid, i32* count);
    f64                 GetTime();
    
    struct Window
    {
        GLFWwindow* handler     = nullptr;
        String      title;
        bool        v_sync      = false;
        CursorMode  cursor_mode = CursorMode::Normal;
    };
}