#pragma once
#ifndef PCH
    #include <upscale_rpc/basic_types.hpp>
#endif

namespace upscale_rpc::util
{
    inline constexpr std::uint8_t byte_count(const size_type_t size_type) noexcept { return +size_type + 1u; }

    inline constexpr std::uint8_t context_id_offset = 2u;
    inline constexpr std::uint8_t count_mask = 0b1111;

    size_type_t storage_bytes_for(const offset_t value) noexcept;
    size_type_t storage_bytes_for(const offset_span_t offsets) noexcept;

    template <typename T>
    struct storage_bytes_detector;

    template <>
    struct storage_bytes_detector<std::uint16_t>
    {
        size_type_t operator()(const std::uint16_t value) const noexcept { return storage_bytes_for(value); }
    };

    template <>
    struct storage_bytes_detector<std::uint32_t>
    {
        size_type_t operator()(const std::uint32_t value) const noexcept { return storage_bytes_for(value); }
    };

    template <>
    struct storage_bytes_detector<std::uint64_t>
    {
        size_type_t operator()(const std::uint64_t value) const noexcept { return storage_bytes_for(value); }
    };

    void align_pointer_forward(auto*& p, const std::uint8_t alignment) noexcept
    {
        auto diff = reinterpret_cast<std::size_t>(p) % alignment;
        if (diff != 0u)
        {
            p += diff;
        }
    }

    void align_pointer_backward(auto*& p, const std::uint8_t alignment) noexcept
    {
        auto diff = reinterpret_cast<std::size_t>(p) % alignment;
        if (diff != 0u)
        {
            p -= alignment - diff;
        }
    }

    void align_pointer(auto*& p, const std::uint8_t alignment) noexcept { align_pointer_forward(p, alignment); }

    void align_offset(auto& offset, const std::uint8_t alignment) noexcept
    {
        const auto diff = offset % alignment;
        if (diff != 0u)
        {
            offset += diff;
        }
    }

    template <typename Array>
    void align_to_array_value(auto& size, const Array& /*sizes*/) noexcept
    {
        align_offset(size, sizeof(typename Array::value_type));
    }
}
