#pragma once

#include "input_enums.h"
#include "input_keycodes.h"
#include "input_modifiers.h"

#ifndef NIT_MAX_INPUT_MODIFIER_TYPES
#define NIT_MAX_INPUT_MODIFIER_TYPES 5
#endif

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


    // First bit of the signature would be used to know if the entity is valid or not
    using InputActionSignature = Bitset<NIT_MAX_INPUT_MODIFIER_TYPES + 1>;

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
        u32 input_modifiers[NIT_MAX_INPUT_MODIFIER_TYPES + 1] = { 0 };
        InputActionSignature signature;
        u32 id = -1;
    };

    void print_key(InputAction* input_action);
    void register_input_action_asset();

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
