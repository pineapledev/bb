#pragma once

#include "input_enums.h"

namespace nit
{

    class InputModifier
    {
    public:
        virtual void modify(Vector4& input_value_, InputType _input_type) const = 0;
    };

    class InputModifierDeadZone : public InputModifier
    {
    public:
        DeadZoneType m_type = DeadZoneType::Radial;
        f32 m_lower_threshold = 0.2f;
        f32 m_upper_threshold = 1.f;

        InputModifierDeadZone(DeadZoneType _type = DeadZoneType::Radial, f32 _lower_threshold = 0.2, f32 _upper_threshold = 1.f);
        virtual void modify(Vector4& input_value_, InputType _input_type) const override;
    };

    class InputModifierScalar : public InputModifier
    {
    public:
        Vector4 m_scalar = V4_ONE;

        InputModifierScalar(f32 _scalar_x = 1.f, f32 _scalar_y = 1.f, f32 _scalar_z = 1.f, f32 _scalar_w = 1.f);
        InputModifierScalar(const Vector4& _scalar);
        virtual void modify(Vector4& input_value_, InputType _input_type) const override;
    };

    class InputModifierAdd : public InputModifier
    {
    public:
        Vector4 m_amount = V4_ONE;

        InputModifierAdd(f32 _amount_x = 1.f, f32 _amount_y = 1.f, f32 _amount_z = 1.f, f32 _amount_w = 1.f);
        InputModifierAdd(const Vector4& _amount);
        virtual void modify(Vector4& input_value_, InputType _input_type) const override;
    };

    class InputModifierNegate : public InputModifier
    {
    public:
        bool m_negate_x = true;
        bool m_negate_y = true;
        bool m_negate_z = true;
        bool m_negate_w = true;

        InputModifierNegate(bool _negate_x = true, bool _negate_y = true, bool _negate_z = true, bool _negate_w = true);
        virtual void modify(Vector4& input_value_, InputType _input_type) const override;
    };


    class InputModifierSwizzleAxis : public InputModifier
    {
    public:
        SwizzleAxisType m_order = SwizzleAxisType::YXZ;

        InputModifierSwizzleAxis(SwizzleAxisType _order);
        virtual void modify(Vector4& input_value_, InputType _input_type) const override;
    };
}