// Upscale RPC library
// Copyright (C) 2022 Flaviu C.
#pragma once
#ifndef PCH
    #include <upscale_rpc/message.hpp>
#endif

namespace upscale_rpc::response
{
    template <const std::uint8_t _Count, const std::uint8_t _Size_type>
    class base: public message::multiple_context_id_base<_Count, _Size_type>
    {
    public:
        using call_result__Count_array = std::array<std::uint8_t, _Count>;

        constexpr base(const bool use_params, const bool use_inline_params = false) noexcept:
            message::multiple_context_id_base<_Count, _Size_type>(marker_t::response, use_inline_params, use_params)
        {
        }

        constexpr const call_result__Count_array& call_result__Counts() const noexcept { return _call_result__Counts; }

        constexpr call_result__Count_array& call_result__Counts() noexcept { return _call_result__Counts; }

    protected:
        call_result__Count_array _call_result__Counts {};
    };

    template <const std::uint8_t _Count, const std::uint8_t _Size_type, const size_t _Param_buffer_size>
    using linked_params = message::linked_params<base, _Count, _Size_type, _Param_buffer_size>;

    template <const std::uint8_t _Count, const std::uint8_t _Size_type>
    using inline_params = message::inline_params<base, _Count, _Size_type>;

    template <const std::uint8_t _Count, const std::uint8_t _Size_type>
    using no_params = message::no_params<base, _Count, _Size_type>;
}
