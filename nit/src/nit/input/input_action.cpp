#include "input_action.h"
#include "nit_pch.h"
#include "core/asset.h"

#ifdef NIT_EDITOR_ENABLED
#include "editor/editor_utils.h"
#endif

namespace nit
{
    void serialize(const InputAction* input_action, YAML::Emitter& emitter)
    {
        emitter << YAML::Key << "key" <<                YAML::Value << enum_to_string<GamepadKeys>(input_action->m_Key);
        emitter << YAML::Key << "paired_input_key_x" << YAML::Value << enum_to_string<GamepadKeys>(input_action->m_PairedInputKeyX);
        emitter << YAML::Key << "paired_input_key_y" << YAML::Value << enum_to_string<GamepadKeys>(input_action->m_PairedInputKeyY);
        emitter << YAML::Key << "paired_input_key_z" << YAML::Value << enum_to_string<GamepadKeys>(input_action->m_PairedInputKeyZ);
        emitter << YAML::Key << "paired_input_key_w" << YAML::Value << enum_to_string<GamepadKeys>(input_action->m_PairedInputKeyW);
        emitter << YAML::Key << "input_type" <<         YAML::Value << enum_to_string<InputType>(input_action->m_InputType);
        emitter << YAML::Key << "trigger_type" <<       YAML::Value << enum_to_string<TriggerType>(input_action->m_TriggerType);
        //emitter << YAML::Key << "input_modifiers" << YAML::Value << material->threshold;
    }

    void deserialize(InputAction* input_action, const YAML::Node& node)
    {
        input_action->m_Key =             enum_from_string<GamepadKeys>(node["key"].as<String>());
        input_action->m_PairedInputKeyX = enum_from_string<GamepadKeys>(node["paired_input_key_x"].as<String>());
        input_action->m_PairedInputKeyY = enum_from_string<GamepadKeys>(node["paired_input_key_y"].as<String>());
        input_action->m_PairedInputKeyZ = enum_from_string<GamepadKeys>(node["paired_input_key_z"].as<String>());
        input_action->m_PairedInputKeyW = enum_from_string<GamepadKeys>(node["paired_input_key_w"].as<String>());
        input_action->m_InputType =       enum_from_string<InputType>(node["input_type"].as<String>());
        input_action->m_TriggerType =     enum_from_string<TriggerType>(node["trigger_type"].as<String>());
    }

#ifdef NIT_EDITOR_ENABLED
    void draw_editor(InputAction* input_action)
    {
        editor_draw_enum_combo("Key", input_action->m_Key);
        editor_draw_enum_combo("Paired Key X", input_action->m_PairedInputKeyX);
        editor_draw_enum_combo("Paired Key Y", input_action->m_PairedInputKeyY);
        editor_draw_enum_combo("Paired Key Z", input_action->m_PairedInputKeyZ);
        editor_draw_enum_combo("Paired Key W", input_action->m_PairedInputKeyW);
        editor_draw_enum_combo("Input Type", input_action->m_InputType);
        editor_draw_enum_combo("Trigger Type", input_action->m_TriggerType);
    }
#endif

    void print_key(InputAction* input_action)
    {
        NIT_LOG_TRACE("%s", enum_to_string<GamepadKeys>(input_action->m_Key).c_str());
    }

    void register_input_action_asset()
    {
        enum_register<GamepadKeys>();
        enum_register_value<GamepadKeys>("GamepadButtonNONE", GamepadKeys::GamepadButtonNONE);
        enum_register_value<GamepadKeys>("GamepadButtonSouth", GamepadKeys::GamepadButtonSouth);
        enum_register_value<GamepadKeys>("GamepadButtonEast", GamepadKeys::GamepadButtonEast);
        enum_register_value<GamepadKeys>("GamepadButtonWest", GamepadKeys::GamepadButtonWest);
        enum_register_value<GamepadKeys>("GamepadButtonNorth", GamepadKeys::GamepadButtonNorth);
        enum_register_value<GamepadKeys>("GamepadButtonLeftBumper", GamepadKeys::GamepadButtonLeftBumper);
        enum_register_value<GamepadKeys>("GamepadButtonRightBumper", GamepadKeys::GamepadButtonRightBumper);
        enum_register_value<GamepadKeys>("GamepadButtonSelect", GamepadKeys::GamepadButtonSelect);
        enum_register_value<GamepadKeys>("GamepadButtonStart", GamepadKeys::GamepadButtonStart);
        enum_register_value<GamepadKeys>("GamepadButtonHome", GamepadKeys::GamepadButtonHome);
        enum_register_value<GamepadKeys>("GamepadButtonLeftThumb", GamepadKeys::GamepadButtonLeftThumb);
        enum_register_value<GamepadKeys>("GamepadButtonRightThumb", GamepadKeys::GamepadButtonRightThumb);
        enum_register_value<GamepadKeys>("GamepadButtonDpadUp", GamepadKeys::GamepadButtonDpadUp);
        enum_register_value<GamepadKeys>("GamepadButtonDpadRight", GamepadKeys::GamepadButtonDpadRight);
        enum_register_value<GamepadKeys>("GamepadButtonDpadDown", GamepadKeys::GamepadButtonDpadDown);
        enum_register_value<GamepadKeys>("GamepadButtonDpadLeft", GamepadKeys::GamepadButtonDpadLeft);
        enum_register_value<GamepadKeys>("GamepadAxisLeftTrigger", GamepadKeys::GamepadAxisLeftTrigger);
        enum_register_value<GamepadKeys>("GamepadAxisRightTrigger", GamepadKeys::GamepadAxisRightTrigger);
        enum_register_value<GamepadKeys>("GamepadAxisLeftX", GamepadKeys::GamepadAxisLeftX);
        enum_register_value<GamepadKeys>("GamepadAxisLeftY", GamepadKeys::GamepadAxisLeftY);
        enum_register_value<GamepadKeys>("GamepadAxisLeft2D", GamepadKeys::GamepadAxisLeft2D);
        enum_register_value<GamepadKeys>("GamepadAxisRightX", GamepadKeys::GamepadAxisRightX);
        enum_register_value<GamepadKeys>("GamepadAxisRightY", GamepadKeys::GamepadAxisRightY);
        enum_register_value<GamepadKeys>("GamepadAxisRight2D", GamepadKeys::GamepadAxisRight2D);
        enum_register_value<GamepadKeys>("GamepadAxisGyroX", GamepadKeys::GamepadAxisGyroX);
        enum_register_value<GamepadKeys>("GamepadAxisGyroY", GamepadKeys::GamepadAxisGyroY);
        enum_register_value<GamepadKeys>("GamepadAxisGyroZ", GamepadKeys::GamepadAxisGyroZ);
        enum_register_value<GamepadKeys>("GamepadAxisGyro3D", GamepadKeys::GamepadAxisGyro3D);
        enum_register_value<GamepadKeys>("GamepadAxisTotalAccelX", GamepadKeys::GamepadAxisTotalAccelX);
        enum_register_value<GamepadKeys>("GamepadAxisTotalAccelY", GamepadKeys::GamepadAxisTotalAccelY);
        enum_register_value<GamepadKeys>("GamepadAxisTotalAccelZ", GamepadKeys::GamepadAxisTotalAccelZ);
        enum_register_value<GamepadKeys>("GamepadAxisTotalAccel3D", GamepadKeys::GamepadAxisTotalAccel3D);
        enum_register_value<GamepadKeys>("GamepadAxisAccelX", GamepadKeys::GamepadAxisAccelX);
        enum_register_value<GamepadKeys>("GamepadAxisAccelY", GamepadKeys::GamepadAxisAccelY);
        enum_register_value<GamepadKeys>("GamepadAxisAccelZ", GamepadKeys::GamepadAxisAccelZ);
        enum_register_value<GamepadKeys>("GamepadAxisAccel3D", GamepadKeys::GamepadAxisAccel3D);
        enum_register_value<GamepadKeys>("GamepadAxisGravX", GamepadKeys::GamepadAxisGravX);
        enum_register_value<GamepadKeys>("GamepadAxisGravY", GamepadKeys::GamepadAxisGravY);
        enum_register_value<GamepadKeys>("GamepadAxisGravZ", GamepadKeys::GamepadAxisGravZ);
        enum_register_value<GamepadKeys>("GamepadAxisGrav3D", GamepadKeys::GamepadAxisGrav3D);
        enum_register_value<GamepadKeys>("GamepadAxisQuatX", GamepadKeys::GamepadAxisQuatX);
        enum_register_value<GamepadKeys>("GamepadAxisQuatY", GamepadKeys::GamepadAxisQuatY);
        enum_register_value<GamepadKeys>("GamepadAxisQuatZ", GamepadKeys::GamepadAxisQuatZ);
        enum_register_value<GamepadKeys>("GamepadAxisQuatW", GamepadKeys::GamepadAxisQuatW);
        enum_register_value<GamepadKeys>("GamepadAxisQuat4D", GamepadKeys::GamepadAxisQuat4D);
        enum_register_value<GamepadKeys>("SwitchButtonCapture", GamepadKeys::SwitchButtonCapture);
        enum_register_value<GamepadKeys>("SwitchButtonLeftSL", GamepadKeys::SwitchButtonLeftSL);
        enum_register_value<GamepadKeys>("SwitchButtonLeftSR", GamepadKeys::SwitchButtonLeftSR);
        enum_register_value<GamepadKeys>("SwitchButtonRightSL", GamepadKeys::SwitchButtonRightSL);
        enum_register_value<GamepadKeys>("SwitchButtonRightSR", GamepadKeys::SwitchButtonRightSR);
        enum_register_value<GamepadKeys>("SwitchButtonSL", GamepadKeys::SwitchButtonSL);
        enum_register_value<GamepadKeys>("SwitchButtonSR", GamepadKeys::SwitchButtonSR);
        enum_register_value<GamepadKeys>("SwitchAxisLeftGyroX", GamepadKeys::SwitchAxisLeftGyroX);
        enum_register_value<GamepadKeys>("SwitchAxisLeftGyroY", GamepadKeys::SwitchAxisLeftGyroY);
        enum_register_value<GamepadKeys>("SwitchAxisLeftGyroZ", GamepadKeys::SwitchAxisLeftGyroZ);
        enum_register_value<GamepadKeys>("SwitchAxisLeftGyro3D", GamepadKeys::SwitchAxisLeftGyro3D);
        enum_register_value<GamepadKeys>("SwitchAxisLeftAccelX", GamepadKeys::SwitchAxisLeftAccelX);
        enum_register_value<GamepadKeys>("SwitchAxisLeftAccelY", GamepadKeys::SwitchAxisLeftAccelY);
        enum_register_value<GamepadKeys>("SwitchAxisLeftAccelZ", GamepadKeys::SwitchAxisLeftAccelZ);
        enum_register_value<GamepadKeys>("SwitchAxisLeftAccel3D", GamepadKeys::SwitchAxisLeftAccel3D);
        enum_register_value<GamepadKeys>("SwitchAxisLeftGravX", GamepadKeys::SwitchAxisLeftGravX);
        enum_register_value<GamepadKeys>("SwitchAxisLeftGravY", GamepadKeys::SwitchAxisLeftGravY);
        enum_register_value<GamepadKeys>("SwitchAxisLeftGravZ", GamepadKeys::SwitchAxisLeftGravZ);
        enum_register_value<GamepadKeys>("SwitchAxisLeftGrav3D", GamepadKeys::SwitchAxisLeftGrav3D);
        enum_register_value<GamepadKeys>("SwitchAxisLeftQuatX", GamepadKeys::SwitchAxisLeftQuatX);
        enum_register_value<GamepadKeys>("SwitchAxisLeftQuatY", GamepadKeys::SwitchAxisLeftQuatY);
        enum_register_value<GamepadKeys>("SwitchAxisLeftQuatZ", GamepadKeys::SwitchAxisLeftQuatZ);
        enum_register_value<GamepadKeys>("SwitchAxisLeftQuatW", GamepadKeys::SwitchAxisLeftQuatW);
        enum_register_value<GamepadKeys>("SwitchAxisLeftQuat4D", GamepadKeys::SwitchAxisLeftQuat4D);
        enum_register_value<GamepadKeys>("DualShockButtonCreate", GamepadKeys::DualShockButtonCreate);
        enum_register_value<GamepadKeys>("DualShockButtonMic", GamepadKeys::DualShockButtonMic);
        enum_register_value<GamepadKeys>("DualShockButtonTouchpadDown", GamepadKeys::DualShockButtonTouchpadDown);
        enum_register_value<GamepadKeys>("DualShockButtonFirstTouchpad", GamepadKeys::DualShockButtonFirstTouchpad);
        enum_register_value<GamepadKeys>("DualShockButtonSecondTouchpad", GamepadKeys::DualShockButtonSecondTouchpad);
        enum_register_value<GamepadKeys>("DualShockAxisFirstTouchpadX", GamepadKeys::DualShockAxisFirstTouchpadX);
        enum_register_value<GamepadKeys>("DualShockAxisFirstTouchpadY", GamepadKeys::DualShockAxisFirstTouchpadY);
        enum_register_value<GamepadKeys>("DualShockAxisFirstTouchpad2D", GamepadKeys::DualShockAxisFirstTouchpad2D);
        enum_register_value<GamepadKeys>("DualShockAxisSecondTouchpadX", GamepadKeys::DualShockAxisSecondTouchpadX);
        enum_register_value<GamepadKeys>("DualShockAxisSecondTouchpadY", GamepadKeys::DualShockAxisSecondTouchpadY);
        enum_register_value<GamepadKeys>("DualShockAxisSecondTouchpad2D", GamepadKeys::DualShockAxisSecondTouchpad2D);
        enum_register_value<GamepadKeys>("DualShockAxisDpadUp", GamepadKeys::DualShockAxisDpadUp);
        enum_register_value<GamepadKeys>("DualShockAxisDpadRight", GamepadKeys::DualShockAxisDpadRight);
        enum_register_value<GamepadKeys>("DualShockAxisDpadDown", GamepadKeys::DualShockAxisDpadDown);
        enum_register_value<GamepadKeys>("DualShockAxisDpadLeft", GamepadKeys::DualShockAxisDpadLeft);
        enum_register_value<GamepadKeys>("DualShockAxisSouth", GamepadKeys::DualShockAxisSouth);
        enum_register_value<GamepadKeys>("DualShockAxisEast", GamepadKeys::DualShockAxisEast);
        enum_register_value<GamepadKeys>("DualShockAxisWest", GamepadKeys::DualShockAxisWest);
        enum_register_value<GamepadKeys>("DualShockAxisNorth", GamepadKeys::DualShockAxisNorth);
        enum_register_value<GamepadKeys>("DualShockAxisLeftBumper", GamepadKeys::DualShockAxisLeftBumper);
        enum_register_value<GamepadKeys>("DualShockAxisRightBumper", GamepadKeys::DualShockAxisRightBumper);

        enum_register<InputType>();
        enum_register_value<InputType>("Digital", InputType::Digital);
        enum_register_value<InputType>("Axis1D", InputType::Axis1D);
        enum_register_value<InputType>("Axis2D", InputType::Axis2D);
        enum_register_value<InputType>("Axis3D", InputType::Axis3D);
        enum_register_value<InputType>("Axis4D", InputType::Axis4D);

        enum_register<TriggerType>();
        enum_register_value<TriggerType>("Down", TriggerType::Down);
        enum_register_value<TriggerType>("Pressed", TriggerType::Pressed);
        enum_register_value<TriggerType>("Released", TriggerType::Released);

        asset_register_type<InputAction>({
            .fn_serialize = serialize,
            .fn_deserialize = deserialize,
            NIT_IF_EDITOR_ENABLED(.fn_draw_editor = draw_editor)
            });
    }


    void on_controller_analog(InputAction* input_action, f32 analog_value)
    {
        InputActionContext context;
        context.inputType = InputType::Axis1D;
        context.inputValue = Vector4(analog_value, 0.f, 0.f, 0.f);
        
        /*
        for (const auto inputModifier : m_InputModifiers)
        {
            inputModifier->Modify(context.inputValue, context.inputType);
        }
        */

        context.controllerId = 1;

        //event_broadcast<const InputActionContext&>(input_action->input_performed_event, context);

        //m_InputPerformedEvent.Broadcast(context);
    }

    void on_controller_vector2(InputAction* input_action, const Vector2& vector2_value)
    {
        InputActionContext context;
        context.inputType = InputType::Axis2D;
        context.inputValue = Vector4(vector2_value.x, vector2_value.y, 0.f, 0.f);

        /*
        for (const auto inputModifier : m_InputModifiers)
        {
            inputModifier->Modify(context.inputValue, context.inputType);
        }
        */

        context.controllerId = 1;
        //event_broadcast<const InputActionContext&>(input_action->input_performed_event, context);
    }

    void on_controller_vector3(InputAction* input_action, const Vector3& vector3_value)
    {
        InputActionContext context;
        context.inputType = InputType::Axis3D;
        context.inputValue = Vector4(vector3_value.x, vector3_value.y, vector3_value.z, 0.f);
        
        /*
        for (const auto inputModifier : m_InputModifiers)
        {
            inputModifier->Modify(context.inputValue, context.inputType);
        }
        */

        context.controllerId = 1;
        //event_broadcast<const InputActionContext&>(input_action->input_performed_event, context);
    }

    void on_controller_vector4(InputAction* input_action, const Vector4& vector4_value)
    {
        InputActionContext context;
        context.inputType = InputType::Axis4D;
        context.inputValue = Vector4(vector4_value.x, vector4_value.y, vector4_value.z, vector4_value.w);

        /*
        for (const auto inputModifier : m_InputModifiers)
        {
            inputModifier->Modify(context.inputValue, context.inputType);
        }
        */

        context.controllerId = 1;
        //event_broadcast<const InputActionContext&>(input_action->input_performed_event, context);
    }

    void on_controller_button_pressed(InputAction* input_action, bool is_repeat)
    {
        if ((input_action->m_TriggerType == TriggerType::Pressed && is_repeat) || input_action->m_TriggerType == TriggerType::Released) return;

        InputActionContext context;
        context.inputType = InputType::Digital;
        context.inputValue = Vector4(1.f, 0.f, 0.f, 0.f);

        /*
        for (const auto inputModifier : m_InputModifiers)
        {
            inputModifier->Modify(context.inputValue, context.inputType);
        }
        */

        context.bIsPressed = true;
        context.bIsRepeat = is_repeat;
        context.controllerId = 1;
        
        event_broadcast<const InputActionContext&>(input_action->input_performed_event, context);
    }

    void on_controller_button_released(InputAction* input_action, bool is_repeat)
    {
        if (input_action->m_TriggerType == TriggerType::Pressed || input_action->m_TriggerType == TriggerType::Down) return;

        InputActionContext context;
        context.inputType = InputType::Digital;
        context.inputValue = Vector4(0.f, 0.f, 0.f, 0.f);

        /*
        for (const auto inputModifier : m_InputModifiers)
        {
            inputModifier->Modify(context.inputValue, context.inputType);
        }
        */

        context.bIsPressed = false;
        context.bIsRepeat = is_repeat;
        context.controllerId = 1;
        event_broadcast<const InputActionContext&>(input_action->input_performed_event, context);
    }
}

