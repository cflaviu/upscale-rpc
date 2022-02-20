#include "upscale_rpc/response.hpp"
#include "upscale_rpc/utils.hpp"
#ifndef PCH
#endif

namespace upscale_rpc::message
{
    response::response(): _context_ids(&_count), _result_counts(&_count) {}

    void response::set_total_result_count() noexcept { _total_result_count = interface::_count != 0u ? sum(_result_counts) : 0u; }

    std::size_t response::primary_storage_size() const noexcept
    {
        return util::context_id_offset + _context_ids.storage_size() + _result_counts.storage_size();
    }

    std::size_t response::storage_size() const noexcept
    {
        std::size_t result = primary_storage_size();
        if (_use_data)
        {
            std::visit(
                [&](auto& sizes) {
                    util::align_to_array_value(result, sizes);
                    result += sizes.storage_size();
                },
                _data);
        }

        return result;
    }

    std::uint8_t* response::serialize_to(std::uint8_t* const s) const noexcept
    {
        s[0] = marker | serialize_primary_data();
        s[1] = serialize_data_details();
        auto p = _context_ids.serialize_to(s + util::context_id_offset);
        p = _result_counts.serialize_to(p);
        if (_use_data)
        {
            std::visit([&](auto& sizes) { p = sizes.serialize_to(p); }, _data);
        }

        return p;
    }

    const std::uint8_t* response::deserialize_from(const std::uint8_t* const s) noexcept
    {
        deserialize_primary_data(s[0]);
        deserialize_data_details(s[1]);
        auto p = _context_ids.deserialize_from(s + util::context_id_offset);
        p = _result_counts.deserialize_from(p);
        _total_result_count = sum(_result_counts);
        if (_use_data)
        {
            p = deserialize_data_from(p, &_total_result_count);
            if (_data_location == data_location_t::linked)
            {
                set_data_offsets(p, &_total_result_count);
            }
        }

        return p;
    }

    bool response::operator==(const response& other) const noexcept
    {
        bool result = basic::operator==(other);
        result &= (_context_ids == other._context_ids) && (_result_counts == other._result_counts);
        return result;
    }
}
