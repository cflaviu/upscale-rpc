// Upscale RPC library
// Copyright (C) 2022 Flaviu C.
#pragma once
#ifndef PCH
    #include <upscale_rpc/basic_types.hpp>
#endif

namespace upscale_rpc::message
{
    template <const std::uint8_t _Count, const std::uint8_t _Size_type>
    class multiple_context_id_base
    {
    public:
        static_assert(_Count != 0u);

        using context_id_array = std::array<context_id_t, _Count>;

        constexpr multiple_context_id_base(const marker_t marker, const bool use_params, const bool use_inline_params = false) noexcept:
            _header(marker, _Size_type, use_inline_params, use_params, _Count)
        {
        }

        header_t header() const noexcept { return _header; }

        constexpr const context_id_array& context_ids() const noexcept { return _context_ids; }

        constexpr context_id_array& context_ids() noexcept { return _context_ids; }
        void set_context_id(const std::uint8_t index, context_id_t id) noexcept { _context_ids[index] = id; }

        bool operator==(const multiple_context_id_base& item) const noexcept = default;

    protected:
        header_t _header;
        context_id_array _context_ids;
    };

    template <const std::uint8_t _Count, const std::uint8_t _Size_type, const size_t _Buffer_size,
              const std::uint8_t _Alignment = sizeof(size_t)>
    class linked_parameter_data
    {
    public:
        static_assert(_Count != 0u);
        static_assert(_Buffer_size != 0u);
        static_assert(_Alignment != 0u);

        using size_type_t = typename restricted_data_traits<_Size_type>::value_type;
        using size_array = std::array<size_type_t, _Count>;
        using data_t = std::span<const std::uint8_t>;
        using index_t = std::uint8_t;

        constexpr const size_array& sizes() const noexcept { return _sizes; }

        data_t buffer() const noexcept { return {_buffer.begin(), _index}; }

        std::uint8_t count() const noexcept { return _count; }

        bool add(data_t data) noexcept
        {
            const auto aligned_index = align_offset_forward(_index, _Alignment);
            const auto new_index = aligned_index + data.size();
            const bool ok = (_count < _sizes.size()) && (new_index < _buffer.size());
            if (ok)
            {
                _sizes[_count] = static_cast<size_type_t>(data.size());
                _offsets[_count] = aligned_index;
                ++_count;
                std::copy(data.begin(), data.end(), _buffer.begin() + aligned_index);
                _index = new_index;
            }

            return ok;
        }

        data_t operator[](const index_t index) const noexcept
        {
            if (index < _count)
            {
                return data_t(_buffer.begin() + _offsets[index], _sizes[index]);
            }

            return {};
        }

        std::size_t bytes_unused() const noexcept { return _buffer.size() - _index; }
        std::size_t bytes_used() const noexcept { return sizeof(linked_parameter_data) - bytes_unused(); }

        void create_offsets() noexcept
        {
            _index = 0u;
            _count = 0u;
            for (const auto size: _sizes)
            {
                const auto aligned_index = align_offset_forward(_index, _Alignment);
                _offsets[_count] = aligned_index;
                ++_count;
                _index = aligned_index + size;
            }
        }

        bool operator==(const linked_parameter_data& item) const noexcept = default;

    protected:
        using buffer_t = std::array<std::uint8_t, _Buffer_size>;

        size_array _sizes {};
        size_array _offsets {};
        size_t _index {};
        std::uint8_t _count {};
        buffer_t _buffer {};
    };

    template <template <const std::uint8_t, const std::uint8_t> class base, const std::uint8_t _Context_count,
              const std::uint8_t _Param_count, const std::uint8_t _Size_type, const size_t _Param_buffer_size,
              const std::uint8_t _Alignment = sizeof(size_t)>
    class linked_params: public base<_Context_count, _Size_type>
    {
    public:
        static_assert(_Context_count != 0u);
        static_assert(_Param_count != 0u);
        static_assert(_Param_buffer_size != 0u);
        static_assert(_Alignment != 0u);

        using param_data_t = linked_parameter_data<_Param_count, _Size_type, _Param_buffer_size, _Alignment>;
        using base_t = base<_Context_count, _Size_type>;

        constexpr linked_params() noexcept: base<_Context_count, _Size_type>(true) {}

        const param_data_t& params() const noexcept { return _params; }

        param_data_t& params() noexcept { return _params; }

        typename param_data_t::data_t param(const typename param_data_t::index_t index) const noexcept
        {
#ifdef NDEBUG
            return _params[index];

#else
            return _params.at(index);
#endif
        }

        std::size_t bytes_used() const noexcept { return sizeof(linked_params) - _params.bytes_unused(); }

        bool operator==(const linked_params& item) const noexcept = default;

    protected:
        param_data_t _params {};
    };

    template <template <const std::uint8_t, const std::uint8_t> class base, const std::uint8_t _Context_count,
              const std::uint8_t _Param_count, const std::uint8_t _Size_type>
    class inline_params: public base<_Context_count, _Size_type>
    {
    public:
        using param_data_t = typename data_traits<_Size_type>::value_type;
        using param_data_array = std::array<param_data_t, _Param_count>;
        using index_t = std::uint8_t;

        constexpr inline_params() noexcept: base<_Context_count, _Size_type>(true, true) {}

        const param_data_array& params() const noexcept { return _params; }

        param_data_array& params() noexcept { return _params; }

        param_data_t params(const index_t index) const noexcept { return _params[index]; }

        param_data_t& params(const index_t index) noexcept { return _params[index]; }

        bool operator==(const inline_params&) const noexcept = default;

    protected:
        param_data_array _params {};
    };

    template <template <const std::uint8_t, const std::uint8_t> class base, const std::uint8_t _Count, const std::uint8_t _Size_type>
    using no_params = base<_Count, _Size_type>;

    template <typename T>
    c_raw_data_t c_raw_data_of(const T& item) noexcept
    {
        return {reinterpret_cast<const std::uint8_t*>(&item), sizeof(T)};
    }

    template <typename T>
    raw_data_t raw_data_of(T& item) noexcept
    {
        return {reinterpret_cast<std::uint8_t*>(&item), sizeof(T)};
    }
}
