// Upscale RPC library
// Copyright (C) 2022 Flaviu C.
#include <catch2/catch_all.hpp>
#include <iostream>
#include <memory>
#include <upscale_rpc/request.hpp>
#include <vector>

namespace upscale_rpc::request
{
    TEST_CASE("Check request cancel")
    {
        cancel<3u> cancel_request {};
        auto rs = sizeof(cancel_request);
        std::cout << "size-of request_cancel: " << rs << '\n';

        cancel_request.set_context_id(0u, 0xAAAAu);
        cancel_request.set_context_id(1u, 0xBBBBu);
        cancel_request.set_context_id(2u, 0xCCCCu);

        auto raw_data = c_raw_data_of(cancel_request);
        std::vector<std::uint8_t> deserialized_cancel {raw_data.begin(), raw_data.end()};

        decltype(cancel_request) resulted_cancel {};
        auto resulted_raw_data = raw_data_of(resulted_cancel);
        std::copy(deserialized_cancel.begin(), deserialized_cancel.end(), resulted_raw_data.begin());

        REQUIRE(cancel_request == resulted_cancel);
    }
}
