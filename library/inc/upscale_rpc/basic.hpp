#pragma once
#ifndef PCH
    #include <upscale_rpc/array.hpp>
    #include <upscale_rpc/interface.hpp>
#endif

namespace upscale_rpc::message
{
    class basic: public interface
    {
    public:
        bool use_data() const noexcept { return _use_data; }
        void set_use_data(const bool item) noexcept { _use_data = item; }

        size_type_t size_type() const noexcept { return _size_type; }
        void set_size_type(const size_type_t item) noexcept { _size_type = item; }

        data_location_t data_location() const noexcept { return _data_location; }
        void set_data_location(const data_location_t item) noexcept { _data_location = item; }

        const array::data_variant& data() const noexcept { return _data; }
        void set_data(array::data_variant item) { _data = std::move(item); }

        /// @brief Data offsets after deserialization.
        const array::offset& data_offsets() const noexcept { return _data_offsets; }

        uint8_t* serialize(offset_span_t offsets, uint8_t* const buffer_begin, uint8_t* s) noexcept;

    protected:
        bool operator==(const basic& other) const noexcept;
        bool operator!=(const basic& other) const noexcept = default;

        const std::uint8_t* deserialize_data_from(const std::uint8_t* s, const std::uint8_t* count) noexcept;
        std::uint8_t serialize_primary_data() const noexcept;
        std::uint8_t serialize_data_details() const noexcept;
        void deserialize_primary_data(const std::uint8_t input) noexcept;
        void deserialize_data_details(const std::uint8_t input) noexcept;
        const std::uint8_t* set_data_offsets(const std::uint8_t* s, const std::uint8_t* count) noexcept;

        array::offset _data_offsets {};
        array::data_variant _data {};
        bool _use_data {}; // use params/results
        data_location_t _data_location {};
        size_type_t _size_type {};
    };
}
