// Upscale RPC library
// Copyright (C) 2022 Flaviu C.
#include <catch2/catch_all.hpp>
#include <iostream>
#include <upscale_rpc/response.hpp>
#include <vector>

// namespace upscale_rpc::message
//{
//    TEST_CASE("Check response")
//    {
//        response r;
//        auto rs = sizeof(r);
//        std::cout << "size-of response: " << rs << '\n';

//        r.set_count(3u);

//        r.set_context_id(0u, 0xAAAAu);
//        r.set_context_id(1u, 0xBBBBu);
//        r.set_context_id(2u, 0xCCCCu);

//        r.set_result_count(0u, 2u);
//        r.set_result_count(1u, 3u);
//        r.set_result_count(2u, 4u);

//        r.set_total_result_count();

//        r.set_use_data(true);
//        r.set_size_type(size_type_t::_4_bytes);
//        r.set_data_location(data_location_t::linked);

//        array::data_32 data_sizes(r.total_result_count_ptr());
//        data_sizes[0u] = 0xAAAAAAA1u;
//        data_sizes[1u] = 0xAAAAAAA2u;

//        data_sizes[2u] = 0xBBBBBBB1u;
//        data_sizes[3u] = 0xBBBBBBB2u;
//        data_sizes[4u] = 0xBBBBBBB3u;

//        data_sizes[5u] = 0xCCCCCCC1u;
//        data_sizes[6u] = 0xCCCCCCC2u;
//        data_sizes[7u] = 0xCCCCCCC3u;
//        data_sizes[8u] = 0xCCCCCCC4u;

//        r.set_data(std::move(data_sizes));

//        auto s = r.storage_size();
//        std::cout << "storage-size-of response: " << s << '\n';

//        std::vector<std::uint8_t> buffer(s);
//        r.serialize_to(buffer.data());

//        response rr;
//        rr.deserialize_from(buffer.data());

//#if 0
//        bool ok1 = r.context_ids == rr.context_ids;
//        bool ok2 = r.result_counts == rr.result_counts;
//        bool ok3 = r.use_tiny_results ? r.tiny_results == rr.tiny_results : true;
//        bool ok4 = r.use_data_sizes ? r.data_sizes == rr.data_sizes : true;
//#endif

//        if (r.use_data())
//        {
//            std::visit(
//                [](const auto& sizes) {
//                    for (auto s: sizes)
//                    {
//                        std::cout << std::hex << int(s) << '|';
//                    }

//                    std::cout << '\n';
//                },
//                rr.data());

//            if (r.data_location() == data_location_t::linked)
//            {
//                auto& offsets = r.data_offsets();
//                for (auto offset: offsets)
//                {
//                    std::cout << offset << ' ';
//                }

//                std::cout << '\n';
//            }
//        }

//        REQUIRE(r == rr);
//    }
//}
