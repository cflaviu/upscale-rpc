#include "upscale_rpc/basic.hpp"
#ifndef PCH
    #include <cassert>
#endif

namespace upscale_rpc::message
{
    constexpr std::uint8_t size_type_mask = 0b11;
    constexpr std::uint8_t size_type_shift = 4u;
    constexpr std::uint8_t data_location_mask = 1u;
    constexpr std::uint8_t data_location_shift = 1u;
    constexpr std::uint8_t use_data_mask = 1u;

    std::uint8_t basic::serialize_primary_data() const noexcept { return (+_size_type << size_type_shift) | (_count & util::count_mask); }

    std::uint8_t basic::serialize_data_details() const noexcept
    {
        return (+_data_location << data_location_shift) | std::uint8_t(_use_data);
    }

    void basic::deserialize_primary_data(const std::uint8_t input) noexcept
    {
        set_count(input & util::count_mask);
        set_size_type(size_type_t((input >> size_type_shift) & size_type_mask));
    }

    void basic::deserialize_data_details(const std::uint8_t input) noexcept
    {
        set_use_data(bool(input & use_data_mask));
        set_data_location(data_location_t((input >> data_location_shift) & data_location_mask));
    }

    const std::uint8_t* basic::set_data_offsets(const std::uint8_t* const s, const std::uint8_t* count) noexcept
    {
        _data_offsets.set_size_ptr(count);
        auto p = s;
        std::visit(
            [&](const auto& sizes) {
                auto dest = _data_offsets.begin();
                for (auto size: sizes)
                {
                    auto offset = static_cast<std::uint32_t>(std::distance(s, p));
                    util::align_offset(offset, sizeof(std::size_t));
                    *dest = offset;
                    p += size;
                    ++dest;
                }
            },
            _data);

        return p;
    }

    template <typename ArrayT>
    static const std::uint8_t* deserialize_array(array::data_variant& dest, const std::uint8_t* count, const std::uint8_t* s) noexcept
    {
        ArrayT items(count);
        auto result = items.deserialize_from(s);
        dest = std::move(items);
        return result;
    }

    const std::uint8_t* basic::deserialize_data_from(const std::uint8_t* s, const std::uint8_t* count) noexcept
    {
        const std::uint8_t* result;
        switch (_size_type)
        {
            case size_type_t::_1_byte:
            {
                result = deserialize_array<array::data_8>(_data, count, s);
                break;
            }
            case size_type_t::_2_bytes:
            {
                result = deserialize_array<array::data_16>(_data, count, s);
                break;
            }
            case size_type_t::_4_bytes:
            {
                result = deserialize_array<array::data_32>(_data, count, s);
                break;
            }
            case size_type_t::_8_bytes:
            {
                result = deserialize_array<array::data_64>(_data, count, s);
                break;
            }
            default:
            {
                assert(false);
                result = nullptr;
                break;
            }
        }

        return result;
    }

    bool basic::operator==(const basic& other) const noexcept
    {
        bool result = (_count == other._count) && (_size_type == other._size_type) && (_use_data == other._use_data) &&
                      (_data_location == other._data_location);
        if (_use_data)
        {
            result &= (_data == other._data);
        }

        return result;
    }

    template <typename ArrayT>
    static void copy_offsets(auto& item, offset_span_t offsets) noexcept
    {
        ArrayT data {};
        auto dest = data.begin();
        for (const auto offset: offsets)
        {
            *dest = static_cast<typename ArrayT::value_type>(offset);
            ++dest;
        }

        item.set_data(std::move(data));
    }

    uint8_t* basic::serialize(offset_span_t offsets, uint8_t* const buffer_begin, uint8_t* s) noexcept
    {
        _use_data = true;
        _data_location = data_location_t::linked;

        auto offset_size = util::storage_bytes_for(offsets);
        auto offset_storage_size = util::byte_count(offset_size) * offsets.size();

        auto storage_size = primary_storage_size();
        switch (offset_size)
        {
            case size_type_t::_4_bytes:
            {
                util::align_offset(storage_size, 4u);
                copy_offsets<array::data_32>(*this, offsets);
                break;
            }
            case size_type_t::_2_bytes:
            {
                util::align_offset(storage_size, 2u);
                copy_offsets<array::data_16>(*this, offsets);
                break;
            }
            case size_type_t::_1_byte:
            {
                copy_offsets<array::data_8>(*this, offsets);
                break;
            }
            default:
            {
                assert(0);
                break;
            }
        }

        auto destiantion = s - storage_size - offset_storage_size;
        util::align_pointer_backward(destiantion, sizeof(std::size_t));
        assert(buffer_begin <= destiantion);

        static_cast<void>(serialize_to(destiantion));
        return destiantion;
    }
}
