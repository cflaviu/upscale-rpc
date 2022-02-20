#pragma once
#ifndef PCH
//   #include <algorithm>
    #include <cstring>
    #include <upscale_rpc/utils.hpp>
    #include <variant>
#endif

namespace upscale_rpc
{
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
                    util::align_pointer(s, sizeof(T));
                }

                const auto bytes = storage_size();
                std::memcpy(s, _data.data(), bytes);
                return s + bytes;
            }

            const std::uint8_t* deserialize_from(const std::uint8_t* s) noexcept
            {
                if constexpr (sizeof(T) != 1u)
                {
                    util::align_pointer(s, sizeof(T));
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

        using offset = simple<offset_t, 16u>;
        using data_8 = simple<std::uint8_t, 16u>;
        using data_16 = simple<std::uint16_t, 16u>;
        using data_32 = simple<std::uint32_t, 16u>;
        using data_64 = simple<std::uint64_t, 16u>;

        using data_variant = std::variant<data_8, data_16, data_32, data_64>;
    } // namespace array
} // namespace flex_rpc
