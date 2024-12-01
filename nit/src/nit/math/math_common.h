#pragma once

namespace nit
{
    inline constexpr f32 DEGREES_TO_RADIANS_FACTOR = 0.0174533f;
    inline constexpr f32 RADIANS_TO_DEGREES_FACTOR = 57.2958f;

    inline f32 to_radians(f32 degrees) { return degrees * DEGREES_TO_RADIANS_FACTOR; }
    inline f32 to_degrees(f32 radians) { return radians * RADIANS_TO_DEGREES_FACTOR; }
    
    template<typename T>
    T clamp(const T& value, const T& min, const T& max)
    {
        return std::clamp(value, min, max);
    }
    
    template<typename T>
    T abs(const T& val)
    {
        NIT_CHECK_MSG(false, "Not implemented!");
        return val;
    }

    template<>
    inline f32 abs(const f32& val)
    {
        return (val < 0) ? -val : val;
    }
    
    template<typename T>
    T normalize(const T& val)
    {
        NIT_CHECK_MSG(false, "Not implemented!");
        return val;
    }

    template<typename T>
    f32 magnitude(const T& val)
    {
        NIT_CHECK_MSG(false, "Not implemented!");
        return val;
    }

    template<typename T>
    T multiply(const T& a, const T& b)
    {
        NIT_CHECK_MSG(false, "Not implemented!");
        return a;
    }

    template<typename T>
   T divide(const T& a, const T& b)
    {
        NIT_CHECK_MSG(false, "Not implemented!");
        return a;
    }

    template<typename T>
    f32 dot(const T& a, const T& b)
    {
        NIT_CHECK_MSG(false, "Not implemented!");
        return 0;
    }

    template<typename T>
    f32 distance(const T& a, const T& b)
    {
        NIT_CHECK_MSG(false, "Not implemented!");
        return 0;
    }

    inline bool has_decimals(f32 value)
    {
        f64 int_part;
        f64 frac_part = modf(value, &int_part);
        return frac_part != 0.0;
    }

    template<typename T = f32>
    T random_value(const T& left, const T& right)
    {
        std::random_device random_device;
        std::mt19937 random_engine(random_device());
        std::uniform_real_distribution distribution(left, right);
        return distribution(random_engine);
    }

    template <typename T>
    bool epsilon_equal(T a, T b, T epsilon = static_cast<T>(F32_EPSILON))
    {
        return abs(a - b) <= epsilon;
    }
}