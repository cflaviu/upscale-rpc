#include "protocol.hpp"
#ifndef PCH
#endif

namespace flex_rpc
{
    static constexpr std::uint8_t context_id_offset = 2u;
    const std::uint8_t count_mask = 0xFu;

    std::uint8_t storage_bytes_for(const std::uint32_t value) noexcept { return (value < 0x10000u) ? (value < 0x100u) ? 1u : 2u : 4u; }

    std::size_t request::size() const noexcept
    {
        std::size_t result = context_id_offset + sizeof(context_id_t) + object_methods.size();
        std::visit([&](auto& sizes) { result += sizes.size(); }, data_sizes);
        return result;
    }

    namespace primary_byte
    {
        const std::uint8_t byte_count_mask = 3u;
        const std::uint8_t byte_count_shift = 4u;

        static std::uint8_t serialize(const std::uint8_t byte_count, const std::uint8_t count) noexcept
        {
            return ((byte_count & byte_count_mask) << byte_count_shift) | (count & count_mask);
        }

        static void deserialize(const std::uint8_t byte, std::uint8_t& index, std::uint8_t& count) noexcept
        {
            count = byte & count_mask;
            index = (byte >> byte_count_shift) & byte_count_mask;
        }
    }

    const std::uint8_t* item::set_data_offsets(const std::uint8_t* s) noexcept
    {
        auto dest_data_offset_iter = _data_offsets.begin();
        std::visit(
            [&](const auto& sizes) {
                for (auto size: sizes)
                {
                    align_pointer(s, sizeof(std::uint64_t));
                    *dest_data_offset_iter = s;
                    s += size;
                    ++dest_data_offset_iter;
                }
            },
            data_sizes);

        align_pointer(s, sizeof(std::uint64_t));
        *dest_data_offset_iter = s;
        return s;
    }

    const std::uint8_t* item::deserialize_data_sizes_from(const std::uint8_t* s, const std::uint8_t* count,
                                                          const std::uint8_t index) noexcept
    {
        const std::uint8_t* result;
        switch (index)
        {
            case 0u:
            {
                array::data_sizes_8 items(count);
                result = items.deserialize_from(s);
                data_sizes = std::move(items);
                break;
            }
            case 1u:
            {
                array::data_sizes_16 items(count);
                result = items.deserialize_from(s);
                data_sizes = std::move(items);
                break;
            }
            case 2u:
            {
                array::data_sizes_32 items(count);
                result = items.deserialize_from(s);
                data_sizes = std::move(items);
                break;
            }
            default:
            {
                result = nullptr;
                break;
            }
        }

        return result;
    }

    std::uint8_t* request::serialize_to(std::uint8_t* const s) const noexcept
    {
        s[0] = primary_byte::serialize(static_cast<std::uint8_t>(data_sizes.index()), count);
        s[1] = 0u; // reserved
        std::memcpy(s + context_id_offset, &context_id, sizeof(context_id_t));
        auto p = object_methods.serialize_to(s + context_id_offset + sizeof(context_id_t));
        std::visit([&](auto& sizes) { p = sizes.serialize_to(p); }, data_sizes);
        return p;
    }

    const std::uint8_t* request::deserialize_from(const std::uint8_t* s) noexcept
    {
        std::uint8_t index;
        primary_byte::deserialize(*s, index, count);
        std::memcpy(&context_id, s + context_id_offset, sizeof(context_id_t));
        auto p1 = object_methods.deserialize_from(s + context_id_offset + sizeof(context_id_t));
        auto p2 = deserialize_data_sizes_from(p1, &count, index);
        // set_data_offsets(p2);
        return p2;
    }

    bool request::operator==(const request& item) const noexcept
    {
        return (context_id == item.context_id) && (count == item.count) && (object_methods == item.object_methods) &&
               (data_sizes == item.data_sizes);
    }

    size_t request_cancel::size() const noexcept { return context_id_offset + context_ids.size(); }

    std::uint8_t* request_cancel::serialize_to(std::uint8_t* const s) const noexcept
    {
        s[0] = marker | (count & count_mask);
        s[1] = 0u; // reserved
        return context_ids.serialize_to(s + context_id_offset);
    }

    const std::uint8_t* request_cancel::deserialize_from(const std::uint8_t* const s) noexcept
    {
        count = s[0] & count_mask;
        return context_ids.deserialize_from(s + context_id_offset);
    }

    bool request_cancel::operator==(const request_cancel& item) const noexcept
    {
        return (count == item.count) && (context_ids == item.context_ids);
    }

    void response::set_total_result_count() noexcept { total_result_count = item::count != 0u ? sum(result_counts) : 0u; }

    std::size_t response::size() const noexcept
    {
        std::size_t result = context_id_offset + context_ids.size() + result_counts.size();
        if (use_error_ids)
        {
            result += error_ids.size();
        }

        if (use_data_sizes)
        {
            std::visit([&](auto& sizes) { result += sizes.size(); }, data_sizes);
        }

        return result;
    }

    std::uint8_t* response::serialize_to(std::uint8_t* const s) const noexcept
    {
        s[0] = marker | primary_byte::serialize(static_cast<std::uint8_t>(data_sizes.index()), count);
        s[1] = (std::uint8_t(use_data_sizes) << 1u) | std::uint8_t(use_error_ids); // reserved
        auto p = context_ids.serialize_to(s + context_id_offset);
        p = result_counts.serialize_to(p);
        if (use_error_ids)
        {
            p = error_ids.serialize_to(p);
        }

        if (use_data_sizes)
        {
            std::visit([&](auto& sizes) { p = sizes.serialize_to(p); }, data_sizes);
        }

        return p;
    }

    const std::uint8_t* response::deserialize_from(const std::uint8_t* const s) noexcept
    {
        std::uint8_t index;
        primary_byte::deserialize(*s, index, count);
        use_data_sizes = ((s[1] >> 1u) & 1u) != 0u;
        use_error_ids = (s[1] & 1u) != 0u;
        auto p = context_ids.deserialize_from(s + context_id_offset);
        p = result_counts.deserialize_from(p);
        total_result_count = sum(result_counts);
        if (use_error_ids)
        {
            p = error_ids.deserialize_from(p);
        }

        if (use_data_sizes)
        {
            p = deserialize_data_sizes_from(p, &total_result_count, index);
            // set_data_offsets(p);
        }

        return p;
    }

    bool response::operator==(const response& item) const noexcept
    {
        bool same = (count == item.count) && (use_error_ids == item.use_error_ids) && (use_data_sizes == item.use_data_sizes) &&
                    (context_ids == item.context_ids) && (result_counts == item.result_counts);
        if (same)
        {
            if (use_error_ids)
            {
                same &= (error_ids == item.error_ids);
            }

            if (use_data_sizes)
            {
                same &= (data_sizes == item.data_sizes);
            }
        }

        return same;
    }
}
