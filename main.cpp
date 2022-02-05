#include "protocol.hpp"
#include <iostream>
#include <vector>

void test_request()
{
    flex_rpc::request r;
    auto rs = sizeof(r);
    rs = 0;

    r.count = 3;
    r.context_id = 0xB1C0u;

    r.object_methods[0u] = {0u, 1u};
    r.object_methods[1u] = {0u, 2u};
    r.object_methods[2u] = {0u, 3u};

    flex_rpc::array::data_sizes_16 data_sizes(&r.count);
    data_sizes[0u] = 0xBEEF;
    data_sizes[1u] = 0xFACE;
    data_sizes[2u] = 0xCEA0;

    r.data_sizes = data_sizes;

    auto s = r.size();
    std::cout << "request-size" << s << '\n';

    std::vector<std::uint8_t> buffer(s);
    auto end = r.serialize_to(buffer.data());
    *end = 0;

    flex_rpc::request rr;
    rr.deserialize_from(buffer.data());

#if 0
        bool ok1 = r.data_sizes == rr.data_sizes;
        bool ok2 = r.object_methods == rr.object_methods;
#endif

    std::visit(
        [](const auto& sizes) {
            for (auto s: sizes)
            {
                std::cout << std::hex << int(s) << '|';
            }

            std::cout << '\n';
        },
        rr.data_sizes);

    bool ok = (r == rr);
    std::cout << "request-test: " << ok << '\n';
}

void test_response()
{
    flex_rpc::response r;
    auto rs = sizeof(r);
    rs = 0;

    r.use_data_sizes = true;
    r.use_error_ids = false;

    r.count = 3u;

    r.context_ids[0u] = 0xAAAAu;
    r.context_ids[1u] = 0xBBBBu;
    r.context_ids[2u] = 0xCCCCu;

    r.result_counts.set(0u, 2u);
    r.result_counts.set(1u, 3u);
    r.result_counts.set(2u, 4u);

    r.set_total_result_count();

    r.error_ids[0u] = 0u;
    r.error_ids[1u] = 0u;

    r.error_ids[2u] = 1u;
    r.error_ids[3u] = 2u;
    r.error_ids[4u] = 3u;

    r.error_ids[5u] = 4u;
    r.error_ids[6u] = 5u;
    r.error_ids[7u] = 6u;
    r.error_ids[8u] = 7u;

    flex_rpc::array::data_sizes_32 data_sizes(&r.total_result_count);
    data_sizes[0u] = 0xAAAAAAA1u;
    data_sizes[1u] = 0xAAAAAAA2u;

    data_sizes[2u] = 0xBBBBBBB1u;
    data_sizes[3u] = 0xBBBBBBB2u;
    data_sizes[4u] = 0xBBBBBBB3u;

    data_sizes[5u] = 0xCCCCCCC1u;
    data_sizes[6u] = 0xCCCCCCC2u;
    data_sizes[7u] = 0xCCCCCCC3u;
    data_sizes[8u] = 0xCCCCCCC4u;

    r.data_sizes = data_sizes;

    auto s = r.size();

    std::vector<std::uint8_t> buffer(s);
    r.serialize_to(buffer.data());
    std::cout << "response-size: " << std::dec << s << '\n';

    flex_rpc::response rr;
    rr.deserialize_from(buffer.data());

#if 0
        bool ok1 = r.context_ids == rr.context_ids;
        bool ok2 = r.result_counts == rr.result_counts;
        bool ok3 = r.use_error_ids ? r.error_ids == rr.error_ids : true;
        bool ok4 = r.use_data_sizes ? r.data_sizes == rr.data_sizes : true;
#endif

    if (r.use_data_sizes)
    {
        std::visit(
            [](const auto& sizes) {
                for (auto s: sizes)
                {
                    std::cout << std::hex << int(s) << '|';
                }

                std::cout << '\n';
            },
            rr.data_sizes);
    }

    bool ok = (r == rr);
    std::cout << "response-test: " << ok << '\n';
}

int main()
{
    test_request();
    return 0;
}
