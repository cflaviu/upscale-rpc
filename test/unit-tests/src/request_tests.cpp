#include <catch2/catch_all.hpp>
#include <iostream>
#include <upscale_rpc/request.hpp>
#include <vector>

namespace upscale_rpc::message
{
    TEST_CASE("Check request")
    {
        request r;
        auto rs = sizeof(r);
        std::cout << "size-of request: " << rs << '\n';

        r.set_count(3u);
        r.set_context_id(0xB1C0u);

        r.set_object_method(0u, {0u, 1u});
        r.set_object_method(1u, {0u, 2u});
        r.set_object_method(2u, {0u, 3u});

        r.set_use_data(true);
        r.set_size_type(size_type_t::_2_bytes);
        r.set_data_location(data_location_t::linked);

        array::data_16 data_sizes(r.count_ptr());
        data_sizes[0u] = 0xBEEF;
        data_sizes[1u] = 0xFACE;
        data_sizes[2u] = 0xCEA0;

        r.set_data(std::move(data_sizes));

        auto s = r.storage_size();
        std::cout << "storage-size-of request: " << s << '\n';

        std::vector<std::uint8_t> buffer(s);
        r.serialize_to(buffer.data());

        request rr;
        rr.deserialize_from(buffer.data());

#if 0
        bool ok1 = r.data() == rr.data();
        bool ok2 = r.object_methods() == rr.object_methods();
#endif

        std::visit(
            [](const auto& sizes) {
                for (auto s: sizes)
                {
                    std::cout << std::hex << int(s) << '|';
                }

                std::cout << '\n';
            },
            rr.data());

        REQUIRE(r == rr);
    }
}
