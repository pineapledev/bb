#pragma once

namespace nit
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

    inline bool HasDecimals(f32 value)
    {
        f64 int_part;
        f64 frac_part = modf(value, &int_part);
        return frac_part != 0.0;
    }

    template<typename T = f32>
    T GetRandomValue(const T& left, const T& right)
    {
        std::random_device random_device;
        std::mt19937 random_engine(random_device());
        std::uniform_real_distribution distribution(left, right);
        return distribution(random_engine);
    }
}