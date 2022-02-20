#include <catch2/catch_all.hpp>
#include <iostream>
#include <upscale_rpc/request_cancel.hpp>
#include <vector>

namespace upscale_rpc::message
{
    TEST_CASE("Check request cancel")
    {
        request_cancel r;
        auto rs = sizeof(r);
        std::cout << "size-of request_cancel: " << rs << '\n';

        r.set_count(3u);

        r.set_context_id(0u, 0xAAAAu);
        r.set_context_id(1u, 0xBBBBu);
        r.set_context_id(2u, 0xCCCCu);

        auto s = r.storage_size();
        std::cout << "storage-size-of request_cancel: " << s << '\n';

        std::vector<std::uint8_t> buffer(s);
        r.serialize_to(buffer.data());

        request_cancel rr;
        rr.deserialize_from(buffer.data());

        REQUIRE(r == rr);
    }
}
