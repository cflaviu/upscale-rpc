#include "upscale_rpc/request_cancel.hpp"
#ifndef PCH
#endif

namespace upscale_rpc::message
{
    request_cancel::request_cancel(): _context_ids(&_count) {}

    std::size_t request_cancel::primary_storage_size() const noexcept { return util::context_id_offset + _context_ids.storage_size(); }
    std::size_t request_cancel::storage_size() const noexcept { return primary_storage_size(); }

    std::uint8_t* request_cancel::serialize_to(std::uint8_t* const s) const noexcept
    {
        s[0] = marker | (_count & util::count_mask);
        s[1] = 0u; // reserved
        return _context_ids.serialize_to(s + util::context_id_offset);
    }

    const std::uint8_t* request_cancel::deserialize_from(const std::uint8_t* const s) noexcept
    {
        _count = s[0] & util::count_mask;
        return _context_ids.deserialize_from(s + util::context_id_offset);
    }

    bool request_cancel::operator==(const request_cancel& item) const noexcept
    {
        return (_count == item._count) && (_context_ids == item._context_ids);
    }
}
