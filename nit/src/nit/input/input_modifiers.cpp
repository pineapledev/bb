#include "input_modifiers.h"
#include "input_keycodes.h"
#include "math/math_common.h"

namespace nit
{

    InputModifierDeadZone::InputModifierDeadZone(DeadZoneType _type /*= EDeadZoneType::Radial*/, f32 _lower_threshold /*= 0.2*/, f32 _upper_threshold /*= 1.f*/)
        : m_type(_type)
        , m_lower_threshold(_lower_threshold)
        , m_upper_threshold(_upper_threshold)
    {}

    void InputModifierDeadZone::modify(Vector4& input_value_, InputType _input_type) const
    {
        if (_input_type == InputType::Digital) return;

        auto dead_zone_lambda = [this](const f32 _axis_val)
        {
            return min(1.f, (max(0.f, abs(_axis_val) - m_lower_threshold) / (m_upper_threshold - m_lower_threshold))) * sign(_axis_val);
        };

        switch (m_type)
        {
        case DeadZoneType::Axial:
            input_value_.x = dead_zone_lambda(input_value_.x);
            input_value_.y = dead_zone_lambda(input_value_.y);
            input_value_.z = dead_zone_lambda(input_value_.z);
            input_value_.w = dead_zone_lambda(input_value_.w);
            break;
        case DeadZoneType::Radial:
            if (_input_type == InputType::Axis2D)
            {
                Vector2 new_value = Vector2(input_value_.x, input_value_.y);
                new_value = new_value.x == 0.f && new_value.y == 0.f ? new_value : normalize(new_value);
                new_value = new_value * dead_zone_lambda(magnitude(input_value_));
                input_value_.x = new_value.x;
                input_value_.y = new_value.y;
            }
            else if (_input_type == InputType::Axis3D)
            {
                Vector3 new_value = Vector3(input_value_.x, input_value_.y, input_value_.z);
                new_value = new_value.x == 0.f && new_value.y == 0.f && new_value.z == 0.f ? new_value : normalize(new_value);
                new_value *= dead_zone_lambda(magnitude(new_value));
                input_value_.x = new_value.x;
                input_value_.y = new_value.y;
                input_value_.z = new_value.z;
            }
            else if (_input_type == InputType::Axis4D)
            {
                input_value_ = input_value_.x == 0.f && input_value_.y == 0.f && input_value_.z == 0.f && input_value_.w == 0.f ? input_value_ : normalize(input_value_) * dead_zone_lambda(magnitude(input_value_));
            }
            else
            {
                input_value_.x = dead_zone_lambda(input_value_.x);
            }
            break;
        }
    }

    InputModifierScalar::InputModifierScalar(f32 _scalar_x /*= 1.f*/, f32 _scalar_y /*= 1.f*/, f32 _scalar_z /*= 1.f*/, f32 _scalar_w /*= 1.f*/)
        : m_scalar(Vector4(_scalar_x, _scalar_y, _scalar_z, _scalar_w))
    {}

    InputModifierScalar::InputModifierScalar(const Vector4& _scalar)
        : m_scalar(_scalar)
    {}

    void InputModifierScalar::modify(Vector4& input_value_, InputType _input_type) const
    {
        if (_input_type == InputType::Digital) return;

        input_value_ = input_value_ * m_scalar;
    }

    InputModifierAdd::InputModifierAdd(f32 _amount_x /*= 1.f*/, f32 _amount_y /*= 1.f*/, f32 _amount_z /*= 1.f*/, f32 _amount_w /*= 1.f*/)
        : m_amount(Vector4(_amount_x, _amount_y, _amount_z, _amount_w))
    {

    }

    InputModifierAdd::InputModifierAdd(const Vector4& _amount)
        : m_amount(_amount)
    {}

    void InputModifierAdd::modify(Vector4& input_value_, InputType _input_type) const
    {
        if (_input_type == InputType::Digital) return;

        input_value_ = input_value_ + m_amount;
    }

    InputModifierNegate::InputModifierNegate(bool _negate_x /*= true*/, bool _negate_y /*= true*/, bool _negate_z /*= true*/, bool _negate_w /*= true*/)
        : m_negate_x(_negate_x)
        , m_negate_y(_negate_y)
        , m_negate_z(_negate_z)
        , m_negate_w(_negate_w)
    {}

    void InputModifierNegate::modify(Vector4& input_value_, InputType _input_type) const
    {
        if (_input_type == InputType::Digital) return;

        input_value_ = input_value_ * Vector4(m_negate_x ? -1.f : 1.f, m_negate_y ? -1.f : 1.f, m_negate_z ? -1.f : 1.f, m_negate_w ? -1.f : 1.f);
    }

    InputModifierSwizzleAxis::InputModifierSwizzleAxis(SwizzleAxisType _order)
        : m_order(_order)
    {}

    void InputModifierSwizzleAxis::modify(Vector4& input_value_, InputType _input_type) const
    {
        if (_input_type == InputType::Digital) return;

        Vector4 aux_value = input_value_;
        switch (m_order)
        {
        case SwizzleAxisType::XZY:
            input_value_.y = aux_value.z;
            input_value_.z = aux_value.y;

            break;
        case SwizzleAxisType::YXZ:
            input_value_.x = aux_value.y;
            input_value_.y = aux_value.x;

            break;
        case SwizzleAxisType::YZX:
            input_value_.x = aux_value.y;
            input_value_.y = aux_value.z;
            input_value_.z = aux_value.x;

            break;
        case SwizzleAxisType::ZXY:
            input_value_.x = aux_value.z;
            input_value_.y = aux_value.x;
            input_value_.z = aux_value.y;

            break;
        case SwizzleAxisType::ZYX:
            input_value_.x = aux_value.z;
            input_value_.z = aux_value.x;

            break;
        }
    }

}
