#pragma once

#include "input_enums.h"
#include "input_keycodes.h"

namespace nit
{
    // ------------------------------
    // CONSTRUCTORS
    // ------------------------------

    template<typename T>
    using FnModify = void (*) (const T*, Vector4&, InputType);

    using FnInvokeModify = Function<void(const void*, Vector4&, InputType)>;

    struct InputModifierInfo
    {
        GamepadKeys action_key = GamepadKeys::GamepadButtonNONE;

        Type* type;
        FnInvokeModify fn_invoke_modify;
    };

    struct InputModifierDeadZone
    {
        DeadZoneType deadzone_type = DeadZoneType::Radial;
        f32 lower_threshold = 0.2f;
        f32 upper_threshold = 1.f;
    };

    struct InputModifierScalar
    {
        Vector4 scalar = V4_ONE;
    };

    struct InputModifierAdd
    {
        Vector4 amount = V4_ONE;
    };

    struct InputModifierNegate
    {
        bool negate_x = true;
        bool negate_y = true;
        bool negate_z = true;
        bool negate_w = true;
    };

    struct InputModifierSwizzleAxis
    {
        SwizzleAxisType order = SwizzleAxisType::YXZ;
    };

    // ------------------------------
    // MODIFY INPUTS
    // ------------------------------

    void modify_input(const InputModifierDeadZone* input_modifier, Vector4& input_value, InputType input_type);
    void modify_input(const InputModifierScalar* input_modifier, Vector4& input_value, InputType input_type);
    void modify_input(const InputModifierAdd* input_modifier, Vector4& input_value, InputType input_type);
    void modify_input(const InputModifierNegate* input_modifier, Vector4& input_value, InputType input_type);
    void modify_input(const InputModifierSwizzleAxis* input_modifier, Vector4& input_value, InputType input_type);

    // ------------------------------
    // EDITOR AND ASSET THINGIES
    // ------------------------------

    void register_deadzone_input_modifier();
    void register_scalar_input_modifier();
    void register_add_input_modifier();
    void register_negate_input_modifier();
    void register_swizzle_input_modifier();
}