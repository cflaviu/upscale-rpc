#include "upscale_rpc/request.hpp"
#include "upscale_rpc/utils.hpp"
#ifndef PCH
#endif

namespace upscale_rpc::message
{
    request::request(): _object_methods(&_count) {}

    std::size_t request::primary_storage_size() const noexcept
    {
        return util::context_id_offset + sizeof(context_id_t) + _object_methods.storage_size();
    }

    std::size_t request::storage_size() const noexcept
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

    std::uint8_t* request::serialize_to(std::uint8_t* const s) const noexcept
    {
        s[0] = serialize_primary_data();
        s[1] = serialize_data_details();
        std::memcpy(s + util::context_id_offset, &_context_id, sizeof(context_id_t));
        auto p = _object_methods.serialize_to(s + util::context_id_offset + sizeof(context_id_t));
        if (_use_data)
        {
            std::visit([&](auto& sizes) { p = sizes.serialize_to(p); }, _data);
        }

        return p;
    }

    const std::uint8_t* request::deserialize_from(const std::uint8_t* s) noexcept
    {
        deserialize_primary_data(s[0]);
        deserialize_data_details(s[1]);
        std::memcpy(&_context_id, s + util::context_id_offset, sizeof(context_id_t));
        auto p = _object_methods.deserialize_from(s + util::context_id_offset + sizeof(context_id_t));
        if (_use_data)
        {
            p = deserialize_data_from(p, &_count);
            if (_data_location == data_location_t::linked)
            {
                set_data_offsets(p, &_count);
            }
        }

        return p;
    }

    bool request::operator==(const request& other) const noexcept
    {
        bool result = basic::operator==(other);
        result &= (_context_id == other._context_id) && (_object_methods == other._object_methods);
        return result;
    }
}
