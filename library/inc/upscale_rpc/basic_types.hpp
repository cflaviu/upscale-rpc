// Upscale RPC library
// Copyright (C) 2022 Flaviu C.
#pragma once
#ifndef PCH
    #include <array>
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

    template <typename _Array>
    constexpr std::span<const std::uint8_t> to_span(_Array& array) noexcept
    {
        return {reinterpret_cast<std::uint8_t*>(array.data()), array.size() * sizeof(typename _Array::value_type)};
    }

    template <typename _Array>
    constexpr std::span<const std::uint8_t> to_cspan(_Array& array) noexcept
    {
        return {reinterpret_cast<const std::uint8_t*>(array.data()), array.size() * sizeof(typename _Array::value_type)};
    }

    constexpr auto align_offset_forward(auto offset, const std::uint8_t alignment) noexcept
    {
        return (offset + (alignment - 1u)) & -alignment;
    }

    // @@ not used yet
    enum class size_type_t : std::uint8_t
    {
        _1_byte,
        _2_bytes,
        _4_bytes,
        _8_bytes,
    };

    enum class marker_t : std::uint8_t
    {
        request,
        request_cancel,
        response,
    };

    enum class params_type_t : std::uint8_t
    {
        none,
        linked,
        _inline,
    };

    struct header_t
    {
        std::uint16_t marker : 2, reserved : 3, size_type : 3, use_inline_params : 1, use_params : 1, count : 6;

        constexpr header_t() = default;
        constexpr header_t(const marker_t marker, const std::uint8_t size_type, const bool use_inline_params, const bool use_params,
                           const std::uint8_t count) noexcept:
            marker(+marker),
            reserved(0u),
            size_type(size_type),
            use_inline_params(use_inline_params),
            use_params(use_params),
            count(count)
        {
        }

        bool operator==(const header_t&) const noexcept = default;
    };

    using context_id_t = std::uint16_t;
    using object_id_t = std::uint8_t;
    using method_id_t = std::uint8_t;
    using object_method_id_t = std::pair<object_id_t, method_id_t>;

    template <typename T>
    struct data_traits_scalar
    {
        using value_type = std::uint8_t;

        static void assign(T& dest, const T src) noexcept { dest = src; }
    };

    template <const std::uint8_t size>
    struct data_traits_array
    {
        using value_type = std::array<std::uint8_t, size>;

        static void assign(value_type& dest, const value_type& src) noexcept { std::copy(src.begin(), src.end(), dest.begin()); }
    };

    template <std::uint8_t size_type>
    struct data_traits;

    template <>
    struct data_traits<0u>: data_traits_scalar<std::uint8_t>
    {
    };

    template <>
    struct data_traits<1u>: data_traits_scalar<std::uint16_t>
    {
    };

    template <>
    struct data_traits<2u>: data_traits_scalar<std::uint32_t>
    {
    };

    template <>
    struct data_traits<3u>: data_traits_scalar<std::uint64_t>
    {
    };

    template <>
    struct data_traits<4u>: data_traits_array<16u>
    {
    };

    template <>
    struct data_traits<5u>: data_traits_array<32u>
    {
    };

    template <>
    struct data_traits<6u>: data_traits_array<64u>
    {
    };

    template <>
    struct data_traits<7u>: data_traits_array<128u>
    {
    };

    template <std::uint8_t size_type>
    struct restricted_data_traits;

    template <>
    struct restricted_data_traits<0u>: data_traits_scalar<std::uint8_t>
    {
    };

    template <>
    struct restricted_data_traits<1u>: data_traits_scalar<std::uint16_t>
    {
    };

    template <>
    struct restricted_data_traits<2u>: data_traits_scalar<std::uint32_t>
    {
    };

    using c_raw_data_t = std::span<const std::uint8_t>;
    using raw_data_t = std::span<std::uint8_t>;
}
