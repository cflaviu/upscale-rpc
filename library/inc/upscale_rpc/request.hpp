#pragma once
#ifndef PCH
    #include <upscale_rpc/basic.hpp>
#endif

namespace upscale_rpc::message
{
    using object_id_t = std::uint8_t;
    using method_id_t = std::uint8_t;

    struct object_method
    {
        object_id_t object_id {};
        method_id_t method_id {};

        using array = array::simple<object_method, 16u>;
    };

    class request: public basic
    {
    public:
        static constexpr std::uint8_t marker = 0u;

        request();

        context_id_t context_id() const noexcept { return _context_id; }
        void set_context_id(const context_id_t item) noexcept { _context_id = item; }

        const object_method::array& object_methods() const noexcept { return _object_methods; }
        void set_object_methods(object_method::array item) { _object_methods = std::move(item); }
        void set_object_method(const std::uint8_t index, const object_method item) noexcept { _object_methods[index] = item; }

        std::size_t primary_storage_size() const noexcept override;
        std::size_t storage_size() const noexcept override;

        std::uint8_t* serialize_to(std::uint8_t* const s) const noexcept override;
        const std::uint8_t* deserialize_from(const std::uint8_t* s) noexcept override;

        bool operator==(const request& other) const noexcept;
        bool operator!=(const request& other) const noexcept = default;

    protected:
        object_method::array _object_methods;
        context_id_t _context_id {};
    };
}
