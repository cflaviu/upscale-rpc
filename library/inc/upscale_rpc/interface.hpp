#pragma once
#ifndef PCH
    #include <cstdint>
#endif

namespace upscale_rpc::message
{
    class interface
    {
    public:
        virtual ~interface() = default;

        std::uint8_t count() const noexcept { return _count; }
        void set_count(const std::uint8_t item) noexcept { _count = item; }

        const std::uint8_t* count_ptr() const noexcept { return &_count; }

        virtual std::size_t primary_storage_size() const noexcept = 0;
        virtual std::size_t storage_size() const noexcept = 0;

        virtual std::uint8_t* serialize_to(std::uint8_t* const s) const noexcept = 0;
        virtual const std::uint8_t* deserialize_from(const std::uint8_t* s) noexcept = 0;

    protected:
        std::uint8_t _count {};
    };
}
