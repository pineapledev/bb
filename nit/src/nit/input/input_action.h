#pragma once

#include "input_enums.h"
#include "input_keycodes.h"

namespace nit
{

    constexpr f32 g_LowerDeadzone = 0.2f;

    struct InputContent
    {
        bool lastDigitalValue = false;
        bool currentDigitalValue = false;
        f32 currentAnalogValue = 0.f;
        bool isDirty = false;
    };

    struct InputActionContext
    {
        bool bIsRepeat = false;
        bool bIsPressed = false;
        InputType inputType = InputType::Digital;
        Vector4 inputValue = V4_ZERO;
        i32 controllerId = -1;
    };

    //DECLARE_EVENT_ONE_PARAM(class InputActionInternal, InputPerformedEvent, const InputActionContext&, context)


    struct InputAction
    {
        Event<const InputActionContext&> input_performed_event;
        GamepadKeys m_Key;
        GamepadKeys m_PairedInputKeyX;
        GamepadKeys m_PairedInputKeyY;
        GamepadKeys m_PairedInputKeyZ;
        GamepadKeys m_PairedInputKeyW;
        bool m_IsKeyPressed = false;
        InputType m_InputType = InputType::Digital;
        TriggerType m_TriggerType = TriggerType::Pressed;
        //Array<SharedPtr<InputModifier>> m_InputModifiers;
        u64 id = ++lastId;
        inline static u64 lastId = 0;
    };

    void print_key(InputAction* input_action);
    void register_input_action_asset();

    void on_controller_analog(InputAction* input_action, f32 analog_value);
    void on_controller_vector2(InputAction* input_action, const Vector2& vector2_value);
    void on_controller_vector3(InputAction* input_action, const Vector3& vector3_value);
    void on_controller_vector4(InputAction* input_action, const Vector4& vector4_value);
    void on_controller_button_pressed(InputAction* input_action, bool is_repeat);
    void on_controller_button_released(InputAction* input_action, bool is_repeat);
    /*
    class InputAction
    {
    public:
        InputAction(const String& _keyName, InputType _inputType);
        InputAction(const String& _keyName, const String& _pairedInputKeyNameX, const String& _pairedInputKeyNameY);
        InputAction(const String& _keyName, const String& _pairedInputKeyNameX, const String& _pairedInputKeyNameY, const String& _pairedInputKeyNameZ);
        InputAction(const String& _keyName, const String& _pairedInputKeyNameX, const String& _pairedInputKeyNameY, const String& _pairedInputKeyNameZ, const String& _pairedInputKeyNameW);

        ~InputAction();

        InputPerformedEvent& OnPerformed() { return m_InputPerformedEvent; }
        bool IsKeyPressed() const { return m_IsKeyPressed; }
        const String& GetKeyName() const { return m_KeyName; }
        TriggerType GetTriggerType() const { return m_TriggerType; }
        void SetTriggerType(TriggerType _triggerType) { m_TriggerType = _triggerType; }
        void AddInputModifier(const SharedPtr<InputModifier>& _inputModifier) { m_InputModifiers.push_back(_inputModifier); }
        u64 GetId() { return id; }

        void Update();

        void OnControllerAnalog(i32 _controllerId, f32 _analogValue);
        void OnControllerVector2(i32 _controllerId, const v2& _vector2Value);
        void OnControllerVector3(i32 _controllerId, const v3& _vector3Value);
        void OnControllerVector4(i32 _controllerId, const v4& _vector4Value);
        void OnControllerButtonPressed(i32 _controllerId, bool _bIsRepeat);
        void OnControllerButtonReleased(i32 _controllerId, bool _bIsRepeat);
    private:

        InputPerformedEvent m_InputPerformedEvent;
        const String m_KeyName;
        const String m_PairedInputKeyNameX;
        const String m_PairedInputKeyNameY;
        const String m_PairedInputKeyNameZ;
        const String m_PairedInputKeyNameW;
        bool m_IsKeyPressed = false;
        InputType m_InputType = InputType::Digital;
        TriggerType m_TriggerType = TriggerType::Pressed;
        Array<SharedPtr<InputModifier>> m_InputModifiers;
        u64 id = ++lastId;
        inline static u64 lastId = 0;
    };
    */
}
