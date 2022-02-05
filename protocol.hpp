#pragma once
#ifndef PCH
    #include <algorithm>
    #include <array>
    #include <cstring>
    #include <variant>
#endif

namespace flex_rpc
{
    using object_id_t = std::uint8_t;
    using method_id_t = std::uint8_t;
    using context_id_t = std::uint16_t;
    using error_t = std::uint8_t;
    using offset_t = std::uint16_t;

    std::uint8_t storage_bytes_for(std::uint32_t value) noexcept;

    template <typename T>
    void align_pointer(T*& p, const std::uint8_t alignment) noexcept
    {
        auto diff = reinterpret_cast<std::size_t>(p) % alignment;
        if (diff != 0u)
        {
            p += diff;
        }
    }

    namespace array
    {
        template <const std::size_t N, typename Length = std::uint8_t>
        struct nibble
        {
            using length_t = Length;

            nibble(const length_t* const length) noexcept: _length(length) {}

            length_t length() const noexcept { return *_length; }

            std::size_t size() const noexcept
            {
                auto value = length();
                return value / 2u + value % 2u;
            }

            static constexpr std::size_t max_size() noexcept { return N / 2u; }

            std::uint8_t get(const std::size_t index) const noexcept
            {
                auto value = _data[index / 2u];
                return (index % 2u == 0u ? value >> 4u : value) & 0xFu;
            }

            void set(const std::size_t index, const std::uint8_t value) noexcept
            {
                auto& existing_value = _data[index / 2u];
                existing_value = (index % 2u == 0u) ? (value << 4u) | (existing_value & 0xFu) : (existing_value & 0xF0u) | (value & 0xFu);
            }

            const std::uint8_t* data() const noexcept { return _data.data(); }
            std::uint8_t* data() noexcept { return _data.data(); }

            std::uint8_t operator[](const std::size_t index) const noexcept { return get(index); }

            std::uint8_t* serialize_to(std::uint8_t* const s) const noexcept
            {
                const auto bytes = size();
                std::memcpy(s, _data.data(), bytes);
                return s + bytes;
            }

            const std::uint8_t* deserialize_from(const std::uint8_t* const s) noexcept
            {
                const auto bytes = size();
                std::memcpy(_data.data(), s, bytes);
                return s + bytes;
            }

            bool operator==(const nibble& item) const noexcept
            {
                const auto bytes = size();
                return (bytes == item.size()) && (std::memcmp(_data.data(), item._data.data(), bytes) == 0);
            }

            bool operator!=(const nibble& item) const noexcept = default;

        private:
            using data_t = std::array<std::uint8_t, N / 2u>;
            data_t _data {};
            const length_t* _length;
        };

        template <typename Nibble, typename Sum = std::uint8_t>
        Sum sum(const Nibble& item) noexcept
        {
            const auto length = item.length();
            const auto* iter = item.data();
            constexpr std::uint8_t nibble_mask = 0xFu;
            constexpr std::uint8_t nibble_size = 4u;
            Sum result = 0u;
            for (auto c = length / 2u; c != 0u; --c)
            {
                auto value = *iter;
                ++iter;
                result += (value >> nibble_size) + (value & nibble_mask);
            }

            if ((length % 2u) != 0u)
            {
                result += (*iter >> nibble_size);
            }

            return result;
        }

        template <typename T, const std::size_t N, typename Length = std::uint8_t>
        struct simple: std::array<T, N>
        {
            using base_t = std::array<T, N>;
            using length_t = Length;

            // using base_t::base_t;
            simple(const length_t* const length = nullptr) noexcept: _length(length) {}

            length_t length() const noexcept { return *_length; }
            void set_length(const length_t* const length) noexcept { _length = length; }

            std::size_t size() const noexcept { return length() * sizeof(T); }

            typename base_t::iterator end() noexcept { return base_t::begin() + length(); }
            typename base_t::const_iterator end() const noexcept { return base_t::cbegin() + length(); }
            typename base_t::const_iterator cend() const noexcept { return base_t::cbegin() + length(); }

            std::uint8_t* serialize_to(std::uint8_t* s) const noexcept
            {
                if constexpr (sizeof(T) != 1u)
                {
                    align_pointer(s, sizeof(T));
                }

                const auto bytes = size();
                std::memcpy(s, base_t::data(), bytes);
                return s + bytes;
            }

            const std::uint8_t* deserialize_from(const std::uint8_t* s) noexcept
            {
                if constexpr (sizeof(T) != 1u)
                {
                    align_pointer(s, sizeof(T));
                }

                const auto bytes = size();
                std::memcpy(base_t::data(), s, bytes);
                return s + bytes;
            }

            bool operator==(const simple& item) const noexcept
            {
                const auto bytes = size();
                return (bytes == item.size()) && (std::memcmp(base_t::data(), item.data(), bytes) == 0);
            }

            bool operator!=(const simple& item) const noexcept = default;

        private:
            const length_t* _length {};
        };

        using offset = std::array<const std::uint8_t*, 16u>;
        using data_sizes_8 = simple<std::uint8_t, 16u>;
        using data_sizes_16 = simple<std::uint16_t, 16u>;
        using data_sizes_32 = simple<std::uint32_t, 16u>;

        using data_size_variant = std::variant<data_sizes_8, data_sizes_16, data_sizes_32>;
    } // namespace array

    struct object_method
    {
        object_id_t object_id {};
        method_id_t method_id {};

        using array = flex_rpc::array::simple<object_method, 16u>;
    };

    struct item
    {
        const array::offset& data_offsets() const noexcept { return _data_offsets; }
        const std::uint8_t* set_data_offsets(const std::uint8_t* s) noexcept;

        std::uint8_t count {};
        array::data_size_variant data_sizes {};

    protected:
        const std::uint8_t* deserialize_data_sizes_from(const std::uint8_t* s, const std::uint8_t* count,
                                                        const std::uint8_t index) noexcept;

    private:
        array::offset _data_offsets {};
    };

    struct request: item
    {
        request(): object_methods(&count) {}

        size_t size() const noexcept;

        std::uint8_t* serialize_to(std::uint8_t* const s) const noexcept;
        const std::uint8_t* deserialize_from(const std::uint8_t* s) noexcept;

        bool operator==(const request& item) const noexcept;
        bool operator!=(const request& item) const noexcept = default;

        context_id_t context_id {};
        object_method::array object_methods;
    };

    using context_id_array = array::simple<context_id_t, 16u>;

    struct request_cancel: item
    {
        static constexpr std::uint8_t marker = 0x80u;

        request_cancel(): context_ids(&count) {}

        size_t size() const noexcept;

        std::uint8_t* serialize_to(std::uint8_t* const s) const noexcept;
        const std::uint8_t* deserialize_from(const std::uint8_t* s) noexcept;

        bool operator==(const request_cancel& item) const noexcept;
        bool operator!=(const request_cancel& item) const noexcept = default;

        std::uint8_t count {};
        context_id_array context_ids;
    };

    struct response: item
    {
        using result_count_array = array::nibble<16u>;
        using error_id_array = array::simple<std::uint8_t, 16u>;

        static constexpr std::uint8_t marker = 0x40u;

        response(): context_ids(&count), result_counts(&count), error_ids(&total_result_count) {}

        void set_total_result_count() noexcept;

        size_t size() const noexcept;

        std::uint8_t* serialize_to(std::uint8_t* const s) const noexcept;
        const std::uint8_t* deserialize_from(const std::uint8_t* const s) noexcept;

        bool operator==(const response& item) const noexcept;
        bool operator!=(const response& item) const noexcept = default;

        context_id_array context_ids;
        result_count_array result_counts;
        error_id_array error_ids;
        std::uint8_t total_result_count {};
        bool use_error_ids {};
        bool use_data_sizes {};
    };

} // namespace flex_rpc
