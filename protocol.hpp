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

    std::uint8_t storage_bytes_for(std::uint32_t value) noexcept;

    void align_pointer(auto*& p, const std::uint8_t alignment) noexcept
    {
        auto diff = reinterpret_cast<std::size_t>(p) % alignment;
        if (diff != 0u)
        {
            p += diff;
        }
    }

    void align_offset(auto& offset, const std::uint8_t alignment) noexcept
    {
        const auto diff = offset % alignment;
        if (diff != 0u)
        {
            offset += diff;
        }
    }

    namespace array
    {
        template <const std::size_t N, typename Size = std::uint8_t>
        struct nibble
        {
            using size_t = Size;

            nibble(const size_t* const ptr) noexcept: _size_ptr(ptr) {}

            size_t size() const noexcept { return *_size_ptr; }

            std::size_t storage_size() const noexcept
            {
                auto value = size();
                return (value / 2u) + (value % 2u);
            }

            static constexpr size_t max_size() noexcept { return N / 2u; }

            std::uint8_t get(const size_t index) const noexcept
            {
                auto value = _data[index / 2u];
                return (index % 2u == 0u ? value >> 4u : value) & 0xFu;
            }

            void set(const size_t index, const std::uint8_t value) noexcept
            {
                auto& existing_value = _data[index / 2u];
                existing_value = (index % 2u == 0u) ? (value << 4u) | (existing_value & 0xFu) : (existing_value & 0xF0u) | (value & 0xFu);
            }

            const std::uint8_t* data() const noexcept { return _data.data(); }
            std::uint8_t* data() noexcept { return _data.data(); }

            operator const std::uint8_t *() const noexcept { return _data.data(); }
            operator std::uint8_t *() noexcept { return _data.data(); }

            std::uint8_t operator[](const std::size_t index) const noexcept { return get(index); }

            std::uint8_t* serialize_to(std::uint8_t* const s) const noexcept
            {
                const auto bytes = storage_size();
                std::memcpy(s, _data.data(), bytes);
                return s + bytes;
            }

            const std::uint8_t* deserialize_from(const std::uint8_t* const s) noexcept
            {
                const auto bytes = storage_size();
                std::memcpy(_data.data(), s, bytes);
                return s + bytes;
            }

            bool operator==(const nibble& item) const noexcept
            {
                return (size() == item.size()) && (std::memcmp(_data.data(), item._data.data(), storage_size()) == 0);
            }

            bool operator!=(const nibble& item) const noexcept = default;

        private:
            using data_t = std::array<std::uint8_t, N / 2u>;
            data_t _data {};
            const size_t* _size_ptr;
        };

        template <typename Nibble, typename Sum = std::uint8_t>
        Sum sum(const Nibble& item) noexcept
        {
            constexpr std::uint8_t nibble_mask = 0xFu;
            constexpr std::uint8_t nibble_size = 4u;

            const auto length = item.storage_size();
            const auto* iter = item.data();

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

        template <typename T, const std::size_t N, typename Size = std::uint8_t>
        struct simple
        {
            using internal_array_t = std::array<T, N>;
            using value_type = typename internal_array_t::value_type;
            using iterator = typename internal_array_t::iterator;
            using const_iterator = typename internal_array_t::iterator;
            using size_t = Size;

            simple(const size_t* const ptr = nullptr) noexcept: _size_ptr(ptr) {}

            size_t size() const noexcept { return _size_ptr != nullptr ? *_size_ptr : 0u; }
            void set_size_ptr(const size_t* const ptr) noexcept { _size_ptr = ptr; }

            bool empty() const noexcept { return size() == 0u; }

            std::size_t storage_size() const noexcept { return size() * sizeof(T); }

            auto cbegin() const noexcept { return _data.cbegin(); }
            auto cend() const noexcept { return cbegin() + size(); }

            auto begin() noexcept { return _data.begin(); }
            auto end() noexcept { return begin() + size(); }
            auto begin() const noexcept { return cbegin(); }
            auto end() const noexcept { return cbegin() + size(); }

            const T* data() const noexcept { return _data.data(); }
            T* data() noexcept { return _data.data(); }

            operator const T*() const noexcept { return _data.data(); }
            operator T*() noexcept { return _data.data(); }

            T operator[](const size_t index) const noexcept { return _data[index]; }
            T& operator[](const size_t index) noexcept { return _data[index]; }

            std::uint8_t* serialize_to(std::uint8_t* s) const noexcept
            {
                if constexpr (sizeof(T) != 1u)
                {
                    align_pointer(s, sizeof(T));
                }

                const auto bytes = storage_size();
                std::memcpy(s, _data.data(), bytes);
                return s + bytes;
            }

            const std::uint8_t* deserialize_from(const std::uint8_t* s) noexcept
            {
                if constexpr (sizeof(T) != 1u)
                {
                    align_pointer(s, sizeof(T));
                }

                const auto bytes = storage_size();
                std::memcpy(_data.data(), s, bytes);
                return s + bytes;
            }

            bool operator==(const simple& item) const noexcept
            {
                return (size() == item.size()) && (std::memcmp(_data.data(), item.data(), storage_size()) == 0);
            }

            bool operator!=(const simple& item) const noexcept = default;

            simple& operator=(const simple&) noexcept = default;

        private:
            const size_t* _size_ptr {};
            internal_array_t _data {};
        };

        auto begin(auto& item) noexcept { return item.begin(); }
        auto end(auto& item) noexcept { return item.end(); }
        auto begin(const auto& item) noexcept { return item.cbegin(); }
        auto end(const auto& item) noexcept { return item.cend(); }
        auto cbegin(const auto& item) noexcept { return item.cbegin(); }
        auto cend(const auto& item) noexcept { return item.cend(); }

        using offset = simple<std::uint32_t, 16u>;
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
        const std::uint8_t* set_data_offsets(const std::uint8_t* s, const std::uint8_t* count) noexcept;

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

        size_t storage_size() const noexcept;

        std::uint8_t* serialize_to(std::uint8_t* const s) const noexcept;
        const std::uint8_t* deserialize_from(const std::uint8_t* s) noexcept;

        bool operator==(const request& item) const noexcept;
        bool operator!=(const request& item) const noexcept = default;

        context_id_t context_id {};
        bool use_data_sizes {};
        object_method::array object_methods;
    };

    using context_id_array = array::simple<context_id_t, 16u>;

    struct request_cancel: item
    {
        static constexpr std::uint8_t marker = 0x80u;

        request_cancel(): context_ids(&count) {}

        size_t storage_size() const noexcept;

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
        using tiny_result_array = array::simple<std::uint8_t, 16u>;

        static constexpr std::uint8_t marker = 0x40u;

        response(): context_ids(&count), result_counts(&count), tiny_results(&total_result_count) {}

        void set_total_result_count() noexcept;

        size_t storage_size() const noexcept;

        std::uint8_t* serialize_to(std::uint8_t* const s) const noexcept;
        const std::uint8_t* deserialize_from(const std::uint8_t* const s) noexcept;

        bool operator==(const response& item) const noexcept;
        bool operator!=(const response& item) const noexcept = default;

        context_id_array context_ids;
        result_count_array result_counts;
        tiny_result_array tiny_results;
        std::uint8_t total_result_count {};
        bool use_tiny_results {};
        bool use_data_sizes {};
    };

} // namespace flex_rpc
