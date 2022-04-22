// Upscale RPC library
// Copyright (C) 2022 Flaviu C.
#include <catch2/catch_all.hpp>
#include <upscale_rpc/basic_types.hpp>

namespace upscale_rpc
{
    TEST_CASE("Check header default ctor")
    {
        header_t h {};

        REQUIRE(h.marker == 0u);
        REQUIRE(h.size_type == 0u);
        REQUIRE(h.use_inline_params == false);
        REQUIRE(h.use_params == false);
        REQUIRE(h.count == 0u);
    }

    TEST_CASE("Check header ctor")
    {
        constexpr marker_t marker = marker_t::request;
        constexpr std::uint8_t size_type = 7u;
        constexpr bool use_inline_params = false;
        constexpr bool use_params = true;
        constexpr std::uint8_t count = 4u;

        header_t h(marker, size_type, use_inline_params, use_params, count);

        REQUIRE(h.marker == +marker);
        REQUIRE(h.size_type == size_type);
        REQUIRE(h.use_inline_params == use_inline_params);
        REQUIRE(h.use_params == use_params);
        REQUIRE(h.count == count);
    }
}
