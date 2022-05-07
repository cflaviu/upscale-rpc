// Upscale RPC library
// Copyright (C) 2022 Flaviu C.
#include <catch2/catch_all.hpp>
#include <iostream>
#include <upscale_rpc/response.hpp>
#include <vector>

namespace upscale_rpc::response
{
    TEST_CASE("Check linked-parameters response")
    {
        constexpr std::uint16_t param_buffer_size = 352u;
        constexpr std::uint8_t context_count = 3u;
        constexpr std::uint8_t response_count = 10u;
        constexpr std::uint8_t size_type = 2u;
        using response_t = linked_params<context_count, response_count, size_type, param_buffer_size>;

        response_t response {};
        auto rs = sizeof(response);
        std::cout << "size-of response: " << rs << '\n';

        response.set_context_id(0u, 100u);
        response.set_context_id(1u, 101u);
        response.set_context_id(2u, 102u);

        auto& call_result_counts = response.call_result_counts();
        call_result_counts[0u] = 4u;
        call_result_counts[1u] = 4u;
        call_result_counts[2u] = 2u;

        std::array<std::uint32_t, 10u> context0_result0 {};
        std::fill(context0_result0.begin(), context0_result0.end(), 0xAC0FFE00u);

        std::array<std::uint16_t, 8u> context0_result1 {};
        std::fill(context0_result1.begin(), context0_result1.end(), 0xABBAu);

        std::array<std::uint64_t, 5u> context0_result2 {};
        std::fill(context0_result2.begin(), context0_result2.end(), 0xF1F2F3F4F5F6F7F8u);

        std::array<std::uint8_t, 16u> context0_result3 {};
        std::fill(context0_result3.begin(), context0_result3.end(), 0xF1F2F3F4F5F6F7F8u);

        std::array<std::uint32_t, 10u> context1_result0 {};
        std::fill(context1_result0.begin(), context1_result0.end(), 0xAC0FFE00u);

        std::array<std::uint16_t, 8u> context1_result1 {};
        std::fill(context1_result1.begin(), context1_result1.end(), 0xABBAu);

        std::array<std::uint64_t, 5u> context1_result2 {};
        std::fill(context1_result2.begin(), context1_result2.end(), 0xF1F2F3F4F5F6F7F8u);

        std::array<std::uint8_t, 16u> context1_result3 {};
        std::fill(context1_result3.begin(), context1_result3.end(), 0xF1F2F3F4F5F6F7F8u);

        std::array<std::uint8_t, 16u> context2_result0 {};
        std::fill(context2_result0.begin(), context2_result0.end(), 0xF1F2F3F4F5F6F7F8u);

        std::array<std::uint32_t, 10u> context2_result1 {};
        std::fill(context2_result1.begin(), context2_result1.end(), 0xAC0FFE00u);

        auto& results = response.params();
        REQUIRE(results.add(to_cspan(context0_result0)));
        REQUIRE(results.add(to_cspan(context0_result1)));
        REQUIRE(results.add(to_cspan(context0_result2)));
        REQUIRE(results.add(to_cspan(context0_result3)));

        REQUIRE(results.add(to_cspan(context1_result0)));
        REQUIRE(results.add(to_cspan(context1_result1)));
        REQUIRE(results.add(to_cspan(context1_result2)));
        REQUIRE(results.add(to_cspan(context1_result3)));

        REQUIRE(results.add(to_cspan(context2_result0)));
        REQUIRE(results.add(to_cspan(context2_result1)));

        auto results_buffer_size = results.bytes_used();
        auto results_buffer_unsize = results.bytes_unused();
        std::cout << "results buffer used: " << results_buffer_size << '\n';
        std::cout << "results buffer unused: " << results_buffer_unsize << '\n';

        auto response_bytes_used = response.bytes_used();
        std::cout << "response bytes used: " << response_bytes_used << '\n';

        upscale_rpc::c_raw_data_t serialized_raw_data {reinterpret_cast<const std::uint8_t*>(&response), response_bytes_used};
        std::vector<std::uint8_t> serialized_data {serialized_raw_data.begin(), serialized_raw_data.end()};

        response_t deserialized_response {};
        upscale_rpc::raw_data_t deserialized_raw_data {reinterpret_cast<std::uint8_t*>(&deserialized_response), serialized_data.size()};
        std::copy(serialized_data.begin(), serialized_data.end(), deserialized_raw_data.begin());

        REQUIRE(response.params() == deserialized_response.params());
        REQUIRE(response == deserialized_response);
    }
}
