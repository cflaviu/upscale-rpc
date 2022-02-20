#include "upscale_rpc/utils.hpp"
#ifndef PCH
#endif

namespace upscale_rpc::util
{
    size_type_t storage_bytes_for(const std::uint16_t value) noexcept
    {
        return (value < 0x100u) ? size_type_t::_1_byte : size_type_t::_2_bytes;
    }

    size_type_t storage_bytes_for(const std::uint32_t value) noexcept
    {
        return (value < 0x10000u) ? (value < 0x100u) ? size_type_t::_1_byte : size_type_t::_2_bytes : size_type_t::_4_bytes;
    }

    size_type_t storage_bytes_for(const std::uint64_t value) noexcept
    {
        return (value < 0xFFFFFFFFu) ?
                   (value < 0x10000u) ? (value < 0x100u) ? size_type_t::_1_byte : size_type_t::_2_bytes : size_type_t::_4_bytes :
                   size_type_t::_8_bytes;
    }

    size_type_t storage_bytes_for(const offset_span_t offsets) noexcept
    {
        size_type_t result = size_type_t::_1_byte;
        const storage_bytes_detector<offset_span_t::value_type> detector {};
        for (const auto offset: offsets)
        {
            auto size = detector(offset);
            if (+size > +result)
            {
                result = size;
            }
        }

        return result;
    }
}
