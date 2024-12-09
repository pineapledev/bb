#pragma once

namespace nit
{
    enum class DeadZoneType
    {
        Axial,
        Radial,
    };

    enum class SwizzleAxisType
    {
        YXZ,
        ZYX,
        XZY,
        YZX,
        ZXY,
    };

    enum class InputType
    {
        Digital,
        Axis1D,
        Axis2D,
        Axis3D,
        Axis4D
    };

    enum class TriggerType
    {
        Down,
        Pressed,
        Released
    };

    enum class PairedAxis
    {
        None,
        X,
        Y,
        Z,
        W,
    };

    enum class ControllerType
    {
        None,
        Xbox,
        Switch,
    };

    enum class AffectedTriggers
    {
        Both = 1,
        Left = 2,
        Right = 3
    };
}