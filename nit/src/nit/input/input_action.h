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
        bool last_digital_value = false;
        bool current_digital_value = false;
        f32 current_analog_value = 0.f;
        bool is_dirty = false;
    };

    struct InputActionContext
    {
        bool is_repeat = false;
        bool is_pressed = false;
        InputType input_type = InputType::Digital;
        Vector4 input_value = V4_ZERO;
        i32 player_id = -1;
    };

    //DECLARE_EVENT_ONE_PARAM(class InputActionInternal, InputPerformedEvent, const InputActionContext&, context)


    // First bit of the signature would be used to know if the entity is valid or not
    using InputActionSignature = Bitset<NIT_MAX_INPUT_MODIFIER_TYPES + 1>;

    struct InputAction
    {
        Event<const InputActionContext&> input_performed_event;
        GamepadKeys key;
        GamepadKeys paired_input_key_x;
        GamepadKeys paired_input_key_y;
        GamepadKeys paired_input_key_z;
        GamepadKeys paired_input_key_w;
        bool is_key_pressed = false;
        InputType input_type = InputType::Digital;
        TriggerType trigger_type = TriggerType::Pressed;
        u32 input_modifiers[NIT_MAX_INPUT_MODIFIER_TYPES + 1] = { 0 };
        InputActionSignature signature;
        u32 id = -1;
    };

    void print_key(InputAction* input_action);
    void register_input_action_asset();
}
