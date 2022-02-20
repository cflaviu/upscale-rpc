#pragma once
#ifndef PCH
    #include <cstdint>
    #include <span>
#endif

namespace upscale_rpc
{
    template <typename T>
    constexpr auto operator+(T e) noexcept //-> std::enable_if_t<std::is_enum<T>::value, std::underlying_type_t<T>>
    {
        return static_cast<std::underlying_type_t<T>>(e);
    }

    using context_id_t = std::uint16_t;
    using offset_t = std::uint32_t;
    using offset_span_t = std::span<offset_t, std::dynamic_extent>;

    enum class size_type_t : std::uint8_t
    {
        _1_byte,
        _2_bytes,
        _4_bytes,
        _8_bytes,
    };

    enum class data_location_t : std::uint8_t
    {
        linked,
        inplace,
    };
}
