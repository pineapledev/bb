#pragma once

namespace nit
{
    constexpr i32 g_num_keyboard_keys = 120;
    constexpr i32 g_num_xbox_buttons = 14;
    constexpr i32 g_num_switch_buttons = 20;
    constexpr i32 g_num_xbox_axis = 6;
    constexpr i32 g_num_switch_axis = 4;

    constexpr i32 g_max_input_actions = 200;

    #define GLFW_PRESS 1

    enum InputState
    {
        InputRelease = 0,
        InputPress = 1,
        InputRepeat = 2
    };

    enum HatState
    {
        HatCentered = 0,
        HatUp = 1,
        HatRight = 2,
        HatDown = 4,
        HatLeft = 8,
        HatRightUp = (HatRight | HatUp),
        HatRightDown = (HatRight | HatDown),
        HatLeftUp = (HatLeft | HatUp),
        HatLeftDown = (HatLeft | HatDown)
    };

    enum Key
    {
        KeyUnknown = -1,
        KeySpace = 32,
        KeyApostrophe = 39,  // '
        KeyComma = 44,  // ,
        KeyMinus = 45,  // -
        KeyPeriod = 46,  // .
        KeySlash = 47,  // /
        Key0 = 48,
        Key1 = 49,
        Key2 = 50,
        Key3 = 51,
        Key4 = 52,
        Key5 = 53,
        Key6 = 54,
        Key7 = 55,
        Key8 = 56,
        Key9 = 57,
        KeySemicolon = 59,  // ;
        KeyEqual = 61,  // =
        KeyA = 65,
        KeyB = 66,
        KeyC = 67,
        KeyD = 68,
        KeyE = 69,
        KeyF = 70,
        KeyG = 71,
        KeyH = 72,
        KeyI = 73,
        KeyJ = 74,
        KeyK = 75,
        KeyL = 76,
        KeyM = 77,
        KeyN = 78,
        KeyO = 79,
        KeyP = 80,
        KeyQ = 81,
        KeyR = 82,
        KeyS = 83,
        KeyT = 84,
        KeyU = 85,
        KeyV = 86,
        KeyW = 87,
        KeyX = 88,
        KeyY = 89,
        KeyZ = 90,
        KeyLeftBracket = 91,  // [
        KeyBackSlash = 92,
        KeyRightBracket = 93,  // ]
        KeyGraveAccent = 96,  // `
        KeyWorld1 = 161,  // non-US #1
        KeyWorld2 = 162,  // non-US #2

        // Function keys
        KeyEscape = 256,
        KeyEnter = 257,
        KeyTab = 258,
        KeyBackSpace = 259,
        KeyInsert = 260,
        KeyDelete = 261,
        KeyRight = 262,
        KeyLeft = 263,
        KeyDown = 264,
        KeyUp = 265,
        KeyPageUp = 266,
        KeyPageDown = 267,
        KeyHome = 268,
        KeyEnd = 269,
        KeyCapsLock = 280,
        KeyScrollLock = 281,
        KeyNumLock = 282,
        KeyPrintScreen = 283,
        KeyPause = 284,
        KeyF1 = 290,
        KeyF2 = 291,
        KeyF3 = 292,
        KeyF4 = 293,
        KeyF5 = 294,
        KeyF6 = 295,
        KeyF7 = 296,
        KeyF8 = 297,
        KeyF9 = 298,
        KeyF10 = 299,
        KeyF11 = 300,
        KeyF12 = 301,
        KeyF13 = 302,
        KeyF14 = 303,
        KeyF15 = 304,
        KeyF16 = 305,
        KeyF17 = 306,
        KeyF18 = 307,
        KeyF19 = 308,
        KeyF20 = 309,
        KeyF21 = 310,
        KeyF22 = 311,
        KeyF23 = 312,
        KeyF24 = 313,
        KeyF25 = 314,
        KeyKp0 = 320,
        KeyKp1 = 321,
        KeyKp2 = 322,
        KeyKp3 = 323,
        KeyKp4 = 324,
        KeyKp5 = 325,
        KeyKp6 = 326,
        KeyKp7 = 327,
        KeyKp8 = 328,
        KeyKp9 = 329,
        KeyKpDecimal = 330,
        KeyKpDivide = 331,
        KeyKpMultiply = 332,
        KeyKpSubstract = 333,
        KeyKpAdd = 334,
        KeyKpEnter = 335,
        KeyKpEqual = 336,
        KeyLeftShift = 340,
        KeyLeftControl = 341,
        KeyLeftAlt = 342,
        KeyLeftSuper = 343,
        KeyRightShift = 344,
        KeyRightControl = 345,
        KeyRightAlt = 346,
        KeyRightSuper = 347,
        KeyMenu = 348,
        KeyLast = KeyMenu
    };

    struct KeyboardKeyNames
    {
        inline static const char* KeySpace = "KeySpace";
        inline static const char* KeyApostrophe = "KeyApostrophe";
        inline static const char* KeyComma = "KeyComma";
        inline static const char* KeyMinus = "KeyMinus";
        inline static const char* KeyPeriod = "KeyPeriod";
        inline static const char* KeySlash = "KeySlash";
        inline static const char* Key0 = "Key0";
        inline static const char* Key1 = "Key1";
        inline static const char* Key2 = "Key2";
        inline static const char* Key3 = "Key3";
        inline static const char* Key4 = "Key4";
        inline static const char* Key5 = "Key5";
        inline static const char* Key6 = "Key6";
        inline static const char* Key7 = "Key7";
        inline static const char* Key8 = "Key8";
        inline static const char* Key9 = "Key9";
        inline static const char* KeySemicolon = "KeySemicolon";
        inline static const char* KeyEqual = "KeyEqual";
        inline static const char* KeyA = "KeyA";
        inline static const char* KeyB = "KeyB";
        inline static const char* KeyC = "KeyC";
        inline static const char* KeyD = "KeyD";
        inline static const char* KeyE = "KeyE";
        inline static const char* KeyF = "KeyF";
        inline static const char* KeyG = "KeyG";
        inline static const char* KeyH = "KeyH";
        inline static const char* KeyI = "KeyI";
        inline static const char* KeyJ = "KeyJ";
        inline static const char* KeyK = "KeyK";
        inline static const char* KeyL = "KeyL";
        inline static const char* KeyM = "KeyM";
        inline static const char* KeyN = "KeyN";
        inline static const char* KeyO = "KeyO";
        inline static const char* KeyP = "KeyP";
        inline static const char* KeyQ = "KeyQ";
        inline static const char* KeyR = "KeyR";
        inline static const char* KeyS = "KeyS";
        inline static const char* KeyT = "KeyT";
        inline static const char* KeyU = "KeyU";
        inline static const char* KeyV = "KeyV";
        inline static const char* KeyW = "KeyW";
        inline static const char* KeyX = "KeyX";
        inline static const char* KeyY = "KeyY";
        inline static const char* KeyZ = "KeyZ";
        inline static const char* KeyLeftBracket = "KeyLeftBracket";
        inline static const char* KeyBackSlash = "KeyBackSlash";
        inline static const char* KeyRightBracket = "KeyRightBracket";
        inline static const char* KeyGraveAccent = "KeyGraveAccent";
        inline static const char* KeyWorld1 = "KeyWorld1";
        inline static const char* KeyWorld2 = "KeyWorld2";
        inline static const char* KeyEscape = "KeyEscape";
        inline static const char* KeyEnter = "KeyEnter";
        inline static const char* KeyTab = "KeyTab";
        inline static const char* KeyBackSpace = "KeyBackSpace";
        inline static const char* KeyInsert = "KeyInsert";
        inline static const char* KeyDelete = "KeyDelete";
        inline static const char* KeyRight = "KeyRight";
        inline static const char* KeyLeft = "KeyLeft";
        inline static const char* KeyDown = "KeyDown";
        inline static const char* KeyUp = "KeyUp";
        inline static const char* KeyPageUp = "KeyPageUp";
        inline static const char* KeyPageDown = "KeyPageDown";
        inline static const char* KeyHome = "KeyHome";
        inline static const char* KeyEnd = "KeyEnd";
        inline static const char* KeyCapsLock = "KeyCapsLock";
        inline static const char* KeyScrollLock = "KeyScrollLock";
        inline static const char* KeyNumLock = "KeyNumLock";
        inline static const char* KeyPrintScreen = "KeyPrintScreen";
        inline static const char* KeyPause = "KeyPause";
        inline static const char* KeyF1 = "KeyF1";
        inline static const char* KeyF2 = "KeyF2";
        inline static const char* KeyF3 = "KeyF3";
        inline static const char* KeyF4 = "KeyF4";
        inline static const char* KeyF5 = "KeyF5";
        inline static const char* KeyF6 = "KeyF6";
        inline static const char* KeyF7 = "KeyF7";
        inline static const char* KeyF8 = "KeyF8";
        inline static const char* KeyF9 = "KeyF9";
        inline static const char* KeyF10 = "KeyF10";
        inline static const char* KeyF11 = "KeyF11";
        inline static const char* KeyF12 = "KeyF12";
        inline static const char* KeyF13 = "KeyF13";
        inline static const char* KeyF14 = "KeyF14";
        inline static const char* KeyF15 = "KeyF15";
        inline static const char* KeyF16 = "KeyF16";
        inline static const char* KeyF17 = "KeyF17";
        inline static const char* KeyF18 = "KeyF18";
        inline static const char* KeyF19 = "KeyF19";
        inline static const char* KeyF20 = "KeyF20";
        inline static const char* KeyF21 = "KeyF21";
        inline static const char* KeyF22 = "KeyF22";
        inline static const char* KeyF23 = "KeyF23";
        inline static const char* KeyF24 = "KeyF24";
        inline static const char* KeyF25 = "KeyF25";
        inline static const char* KeyKp0 = "KeyKp0";
        inline static const char* KeyKp1 = "KeyKp1";
        inline static const char* KeyKp2 = "KeyKp2";
        inline static const char* KeyKp3 = "KeyKp3";
        inline static const char* KeyKp4 = "KeyKp4";
        inline static const char* KeyKp5 = "KeyKp5";
        inline static const char* KeyKp6 = "KeyKp6";
        inline static const char* KeyKp7 = "KeyKp7";
        inline static const char* KeyKp8 = "KeyKp8";
        inline static const char* KeyKp9 = "KeyKp9";
        inline static const char* KeyKpDecimal = "KeyKpDecimal";
        inline static const char* KeyKpDivide = "KeyKpDivide";
        inline static const char* KeyKpMultiply = "KeyKpMultiply";
        inline static const char* KeyKpSubstract = "KeyKpSubstract";
        inline static const char* KeyKpAdd = "KeyKpAdd";
        inline static const char* KeyKpEnter = "KeyKpEnter";
        inline static const char* KeyKpEqual = "KeyKpEqual";
        inline static const char* KeyLeftShift = "KeyLeftShift";
        inline static const char* KeyLeftControl = "KeyLeftControl";
        inline static const char* KeyLeftAlt = "KeyLeftAlt";
        inline static const char* KeyLeftSuper = "KeyLeftSuper";
        inline static const char* KeyRightShift = "KeyRightShift";
        inline static const char* KeyRightControl = "KeyRightControl";
        inline static const char* KeyRightAlt = "KeyRightAlt";
        inline static const char* KeyRightSuper = "KeyRightSuper";
        inline static const char* KeyMenu = "KeyMenu";
    };

    enum ModifierKey {
        ModShift = 0x0001,
        ModControl = 0x0002,
        ModAlt = 0x0004,
        ModSuper = 0x0008,
        ModCapsLock = 0x0010,
        ModNumLock = 0x0020
    };

    enum MouseButton {
        MouseButton1 = 0,
        MouseButton2 = 1,
        MouseButton3 = 2,
        MouseButton4 = 3,
        MouseButton5 = 4,
        MouseButton6 = 5,
        MouseButton7 = 6,
        MouseButton8 = 7,
        MouseButtonLast = MouseButton8,
        MouseButtonLeft = MouseButton1,
        MouseButtonRight = MouseButton2,
        MouseButtonMiddle = MouseButton3
    };

    enum MouseAxis {
        MouseAxisDeltaX = 0,
        MouseAxisDeltaY = 1,
        MouseAxisScrollX = 2,
        MouseAxisScrollY = 3,
    };

    enum Joystick
    {
        Joystick1 = 0,
        Joystick2 = 1,
        Joystick3 = 2,
        Joystick4 = 3,
        Joystick5 = 4,
        Joystick6 = 5,
        Joystick7 = 6,
        Joystick8 = 7,
        Joystick9 = 8,
        Joystick10 = 9,
        Joystick11 = 10,
        Joystick12 = 11,
        Joystick13 = 12,
        Joystick14 = 13,
        Joystick15 = 14,
        Joystick16 = 15,
        JoystickLast = Joystick16
    };

    enum class GamepadKeys
    {
        GamepadButtonNONE,
        GamepadButtonSouth,
        GamepadButtonEast,
        GamepadButtonWest,
        GamepadButtonNorth,
        GamepadButtonLeftBumper,
        GamepadButtonRightBumper,
        GamepadButtonSelect,
        GamepadButtonStart,
        GamepadButtonHome,
        GamepadButtonLeftThumb,
        GamepadButtonRightThumb,
        GamepadButtonDpadUp,
        GamepadButtonDpadRight,
        GamepadButtonDpadDown,
        GamepadButtonDpadLeft,
        GamepadAxisLeftTrigger,
        GamepadAxisRightTrigger,
        GamepadAxisLeftX,
        GamepadAxisLeftY,
        GamepadAxisLeft2D,
        GamepadAxisRightX,
        GamepadAxisRightY,
        GamepadAxisRight2D,
        GamepadAxisGyroX,
        GamepadAxisGyroY,
        GamepadAxisGyroZ,
        GamepadAxisGyro3D,
        GamepadAxisTotalAccelX,
        GamepadAxisTotalAccelY,
        GamepadAxisTotalAccelZ,
        GamepadAxisTotalAccel3D,
        GamepadAxisAccelX,
        GamepadAxisAccelY,
        GamepadAxisAccelZ,
        GamepadAxisAccel3D,
        GamepadAxisGravX,
        GamepadAxisGravY,
        GamepadAxisGravZ,
        GamepadAxisGrav3D,
        GamepadAxisQuatX,
        GamepadAxisQuatY,
        GamepadAxisQuatZ,
        GamepadAxisQuatW,
        GamepadAxisQuat4D,
        SwitchButtonCapture,
        SwitchButtonLeftSL,
        SwitchButtonLeftSR,
        SwitchButtonRightSL,
        SwitchButtonRightSR,
        SwitchButtonSL,
        SwitchButtonSR,
        SwitchAxisLeftGyroX,
        SwitchAxisLeftGyroY,
        SwitchAxisLeftGyroZ,
        SwitchAxisLeftGyro3D,
        SwitchAxisLeftAccelX,
        SwitchAxisLeftAccelY,
        SwitchAxisLeftAccelZ,
        SwitchAxisLeftAccel3D,
        SwitchAxisLeftGravX,
        SwitchAxisLeftGravY,
        SwitchAxisLeftGravZ,
        SwitchAxisLeftGrav3D,
        SwitchAxisLeftQuatX,
        SwitchAxisLeftQuatY,
        SwitchAxisLeftQuatZ,
        SwitchAxisLeftQuatW,
        SwitchAxisLeftQuat4D,
        DualShockButtonCreate,
        DualShockButtonMic,
        DualShockButtonTouchpadDown,
        DualShockButtonFirstTouchpad,
        DualShockButtonSecondTouchpad,
        DualShockAxisFirstTouchpadX,
        DualShockAxisFirstTouchpadY,
        DualShockAxisFirstTouchpad2D,
        DualShockAxisSecondTouchpadX,
        DualShockAxisSecondTouchpadY,
        DualShockAxisSecondTouchpad2D,
        DualShockAxisDpadUp,
        DualShockAxisDpadRight,
        DualShockAxisDpadDown,
        DualShockAxisDpadLeft,
        DualShockAxisSouth,
        DualShockAxisEast,
        DualShockAxisWest,
        DualShockAxisNorth,
        DualShockAxisLeftBumper,
        DualShockAxisRightBumper,
    };


    struct WiimoteKeyNames
    {
        inline static const char* Wiimote_DPad_Up = "Wiimote_DPad_Up";
        inline static const char* Wiimote_DPad_Down = "Wiimote_DPad_Down";
        inline static const char* Wiimote_DPad_Right = "Wiimote_DPad_Right";
        inline static const char* Wiimote_DPad_Left = "Wiimote_DPad_Left";
        inline static const char* Wiimote_Plus = "Wiimote_Plus";
        inline static const char* Wiimote_Home = "Wiimote_Home";
        inline static const char* Wiimote_Minus = "Wiimote_Minus";
        inline static const char* Wiimote_Button_A = "Wiimote_Button_A";
        inline static const char* Wiimote_Button_B = "Wiimote_Button_B";
        inline static const char* Wiimote_Button_1 = "Wiimote_Button_1";
        inline static const char* Wiimote_Button_2 = "Wiimote_Button_2";
        inline static const char* Wiimote_PointerVisible = "Wiimote_PointerVisible";
        inline static const char* Wiimote_PointerDistance = "Wiimote_PointerDistance";
        inline static const char* Wiimote_PointerAngle = "Wiimote_PointerAngle";
        inline static const char* Wiimote_PointerX = "Wiimote_PointerX";
        inline static const char* Wiimote_PointerY = "Wiimote_PointerY";
        inline static const char* Wiimote_Pointer2D = "Wiimote_Pointer2D";
        inline static const char* Wiimote_BatteryLevel = "Wiimote_BatteryLevel";
        inline static const char* Nunchuck_Button_C = "Nunchuck_Button_C";
        inline static const char* Nunchuck_Button_Z = "Nunchuck_Button_Z";
        inline static const char* Nunchuck_StickX = "Nunchuck_StickX";
        inline static const char* Nunchuck_StickY = "Nunchuck_StickY";
        inline static const char* Nunchuck_Stick2D = "Nunchuck_Stick2D";
        inline static const char* Wiimote_TiltX = "Wiimote_TiltX";
        inline static const char* Wiimote_TiltY = "Wiimote_TiltY";
        inline static const char* Wiimote_TiltZ = "Wiimote_TiltZ";
        inline static const char* Wiimote_Tilt3D = "Wiimote_Tilt3D";
        inline static const char* Wiimote_RotationRateX = "Wiimote_RotationRateX";
        inline static const char* Wiimote_RotationRateY = "Wiimote_RotationRateY";
        inline static const char* Wiimote_RotationRateZ = "Wiimote_RotationRateZ";
        inline static const char* Wiimote_RotationRate3D = "Wiimote_RotationRate3D";
        inline static const char* Wiimote_GravityX = "Wiimote_GravityX";
        inline static const char* Wiimote_GravityY = "Wiimote_GravityY";
        inline static const char* Wiimote_GravityZ = "Wiimote_GravityZ";
        inline static const char* Wiimote_Gravity3D = "Wiimote_Gravity3D";
        inline static const char* Wiimote_AccelerationX = "Wiimote_AccelerationX";
        inline static const char* Wiimote_AccelerationY = "Wiimote_AccelerationY";
        inline static const char* Wiimote_AccelerationZ = "Wiimote_AccelerationZ";
        inline static const char* Wiimote_Acceleration3D = "Wiimote_Acceleration3D";
    };
}