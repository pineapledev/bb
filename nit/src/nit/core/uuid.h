#pragma once

namespace nit
{
    struct UUID
    {
        u64 data = 0;
        explicit operator u64() const { return data; }
    };

    bool uuid_valid(const UUID& uuid);
    bool operator==(const UUID& a, const UUID& b);
    bool operator!=(const UUID& a, const UUID& b);
    
    inline UUID uuid_generate()
    {
        static std::random_device random_device;
        static std::mt19937_64 random_engine(random_device());
        static std::uniform_int_distribution<u64> distribution(random_device());
        return { distribution(random_engine) };
    }

    void register_uuid_component();

}

template <>
struct std::hash<nit::UUID>
{
    std::size_t operator()(const nit::UUID id) const noexcept
    {
        return hash<nit::u64>()(static_cast<nit::u64>(id));
    }
};