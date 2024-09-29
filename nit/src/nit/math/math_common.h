#pragma once

namespace Nit
{
    inline constexpr f32 DEGREES_TO_RADIANS_FACTOR = 0.0174533f;
    inline constexpr f32 RADIANS_TO_DEGREES_FACTOR = 57.2958f;

    inline f32 ToRadians(f32 degrees) { return degrees * DEGREES_TO_RADIANS_FACTOR; }
    inline f32 ToDegrees(f32 radians) { return radians * RADIANS_TO_DEGREES_FACTOR; }
    
    template<typename T>
    T Clamp(const T& value, const T& min, const T& max)
    {
        return std::clamp(value, min, max);
    }
    
    template<typename T>
    T Abs(const T& val)
    {
        NIT_CHECK_MSG(false, "Not implemented!");
        return val;
    }

    template<>
    inline f32 Abs(const f32& val)
    {
        return (val < 0) ? -val : val;
    }
    
    template<typename T>
    T Normalize(const T& val)
    {
        NIT_CHECK_MSG(false, "Not implemented!");
        return val;
    }

    template<typename T>
    f32 Magnitude(const T& val)
    {
        NIT_CHECK_MSG(false, "Not implemented!");
        return val;
    }

    template<typename T>
    T Multiply(const T& a, const T& b)
    {
        NIT_CHECK_MSG(false, "Not implemented!");
        return a;
    }

    template<typename T>
   T Divide(const T& a, const T& b)
    {
        NIT_CHECK_MSG(false, "Not implemented!");
        return a;
    }

    template<typename T>
    f32 Dot(const T& a, const T& b)
    {
        NIT_CHECK_MSG(false, "Not implemented!");
        return 0;
    }

    template<typename T>
    f32 Distance(const T& a, const T& b)
    {
        NIT_CHECK_MSG(false, "Not implemented!");
        return 0;
    }
}