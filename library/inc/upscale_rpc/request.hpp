// Upscale RPC library
// Copyright (C) 2022 Flaviu C.
#pragma once
#ifndef PCH
    #include <upscale_rpc/message.hpp>
#endif

namespace upscale_rpc::request
{
    template <const std::uint8_t _Count, const std::uint8_t _Size_type>
    class base
    {
    public:
        using object_method_id_array = std::array<object_method_id_t, _Count>;
        using index_t = std::uint8_t;

        constexpr base(const bool use_params, const bool use_inline_params = false) noexcept:
            _header(marker_t::request, _Size_type, use_inline_params, use_params, _Count)
        {
        }

        header_t header() const noexcept { return _header; }

        context_id_t context_id() const noexcept { return _context_id; }

        constexpr const object_method_id_array& object_method_ids() const noexcept { return _object_method_ids; }

        constexpr object_method_id_array& object_method_ids() noexcept { return _object_method_ids; }

        object_method_id_t object_method_id(const index_t index) const noexcept
        {
#ifdef NDEBUG
            return _object_method_ids[index]
#else
            return _object_method_ids.at(index);
#endif
        }

    protected:
        header_t _header;
        context_id_t _context_id;
        object_method_id_array _object_method_ids {};
    };

    template <const std::uint8_t _Count, const std::uint8_t _Size_type, const size_t _Param_buffer_size>
    using linked_params = message::linked_params<base, _Count, _Size_type, _Param_buffer_size>;

    template <const std::uint8_t _Count, const std::uint8_t _Size_type>
    using inline_params = message::inline_params<base, _Count, _Size_type>;

    template <const std::uint8_t _Count, const std::uint8_t _Size_type>
    using no_params = message::no_params<base, _Count, _Size_type>;

    template <const std::uint8_t _Count>
    class cancel: public message::multiple_context_id_base<_Count, 0u>
    {
    public:
        constexpr cancel() noexcept: message::multiple_context_id_base<_Count, 0u>(marker_t::request_cancel, false) {}
    };
}
