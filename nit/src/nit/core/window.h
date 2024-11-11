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
        void        init(const WindowCfg& cfg = {});
        void        finish();
        void        close();
        void        update();
        
        void        set_title(const String& new_title);
        void        set_v_sync(bool enabled);
        void        set_cursor_mode(const CursorMode mode);

        bool        should_close();
        void        retrieve_size(i32* width, i32* height);
        Vector2     get_size();
        float       get_aspect();
        void        get_cursor_position(f64* x, f64* y);
        Vector2     get_cursor_position();
        i32         get_mouse_button(i32 button);
        bool        is_mouse_button_pressed(MouseButton button);
        i32         get_key(i32 key);
        bool        is_key_pressed(Key key);
        const char* get_joystick_name(i32 jid);
        const u8*   get_joystick_buttons(i32 jid, i32* count);
        const f32*  get_joystick_axes(i32 jid, i32* count);
        f64         get_time();
    }
}