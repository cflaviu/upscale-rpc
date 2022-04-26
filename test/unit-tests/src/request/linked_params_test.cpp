// Upscale RPC library
// Copyright (C) 2022 Flaviu C.
#include <catch2/catch_all.hpp>
#include <iostream>
#include <upscale_rpc/request.hpp>
#include <vector>

namespace upscale_rpc::request
{
    TEST_CASE("Check linked-parameters request")
    {
        constexpr std::uint8_t param_buffer_size = 128u;
        constexpr std::uint8_t call_count = 4u;
        constexpr std::uint8_t size_type = 2u;
        using request_t = linked_params<call_count, size_type, param_buffer_size>;

        request_t request {};

        request.set_context_id(10u);

        request.object_method_id(0u) = object_method_id_t {0u, 1u};
        request.object_method_id(1u) = object_method_id_t {0u, 2u};
        request.object_method_id(2u) = object_method_id_t {0u, 3u};
        request.object_method_id(3u) = object_method_id_t {0u, 4u};

        std::array<std::uint32_t, 10u> param0 {};
        std::fill(param0.begin(), param0.end(), 0xAC0FFE00u);

        std::array<std::uint16_t, 8u> param1 {};
        std::fill(param1.begin(), param1.end(), 0xABBAu);

        std::array<std::uint64_t, 5u> param2 {};
        std::fill(param2.begin(), param2.end(), 0xF1F2F3F4F5F6F7F8u);

        std::array<std::uint8_t, 16u> param3 {};
        std::fill(param2.begin(), param2.end(), 0xF1F2F3F4F5F6F7F8u);

        auto& params = request.params();

        REQUIRE(params.add(to_cspan(param0)));
        REQUIRE(params.add(to_cspan(param1)));
        REQUIRE(params.add(to_cspan(param2)));
        REQUIRE(params.add(to_cspan(param3)));

        REQUIRE(params.count() == 4u);

        auto total_param_size = std::accumulate(params.sizes().begin(), params.sizes().end(), 0u);
        REQUIRE(total_param_size <= param_buffer_size);

        auto verifier = [&params](const std::uint8_t id, const auto& array_item)
        {
            using array_type = typename std::remove_cvref<decltype(array_item)>::type;
            using value_type = typename array_type::value_type;

            auto result = params[id];
            auto expected = std::span<const std::uint8_t> {reinterpret_cast<const std::uint8_t*>(array_item.data()),
                                                           array_item.size() * sizeof(value_type)};
            REQUIRE(std::equal(result.begin(), result.end(), expected.begin()));
        };

        verifier(0u, param0);
        verifier(1u, param1);
        verifier(2u, param2);
        verifier(3u, param3);

        auto raw_data = c_raw_data_of(request);

        auto deserialized_header = *reinterpret_cast<const header_t*>(raw_data.data());

        const std::vector<std::uint8_t> deserialized_request {raw_data.begin(), raw_data.end()};

        REQUIRE(deserialized_header.count == params.count());
    }
}
