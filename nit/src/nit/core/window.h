#pragma once
#include "key_codes.h"

struct GLFWwindow;

namespace nit
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
        String      title           = "Window :3";
        i32         width           = 1280;
        i32         height          = 720;
        bool        v_sync          = false;
        bool        start_maximized = true;
        CursorMode  cursor_mode     = CursorMode::Normal;
        bool        render_api_logs = false;
    };

    struct Window
    {
        GLFWwindow* handler     = nullptr;
        String      title;
        bool        v_sync      = false;
        CursorMode  cursor_mode = CursorMode::Normal;
    };

    namespace window
    {
        void        set_instance(Window* window_instance);
        void        Init(const WindowCfg& cfg = {});
        void        Finish();
        void        Close();
        void        Update();
        
        void        SetTitle(const String& new_title);
        void        SetVSync(bool enabled);
        void        SetCursorMode(const CursorMode mode);

        bool        ShouldClose();
        void        RetrieveSize(i32* width, i32* height);
        Vector2     GetSize();
        float       GetAspect();
        void        GetCursorPosition(f64* x, f64* y);
        Vector2     GetCursorPosition();
        i32         GetMouseButton(i32 button);
        bool        IsMouseButtonPressed(MouseButton button);
        i32         GetKey(i32 key);
        bool        IsKeyPressed(Key key);
        const char* GetJoystickName(i32 jid);
        const u8*   GetJoystickButtons(i32 jid, i32* count);
        const f32*  GetJoystickAxes(i32 jid, i32* count);
        f64         GetTime();
    }
}