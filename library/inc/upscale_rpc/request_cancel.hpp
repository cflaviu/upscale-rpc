#pragma once
#ifndef PCH
    #include <upscale_rpc/array.hpp>
    #include <upscale_rpc/interface.hpp>
#endif

namespace upscale_rpc::message
{
    using context_id_array = array::simple<context_id_t, 16u>;

    class request_cancel: public interface
    {
    public:
        static constexpr std::uint8_t marker = 0x80u;

        request_cancel();

        const context_id_array& context_ids() const noexcept { return _context_ids; }
        void set_context_ids(context_id_array item) { _context_ids = std::move(item); }
        void set_context_id(const std::uint8_t index, context_id_t item) noexcept { _context_ids[index] = item; }

        std::size_t primary_storage_size() const noexcept override;
        std::size_t storage_size() const noexcept override;

        std::uint8_t* serialize_to(std::uint8_t* const s) const noexcept override;
        const std::uint8_t* deserialize_from(const std::uint8_t* s) noexcept override;

        bool operator==(const request_cancel& item) const noexcept;
        bool operator!=(const request_cancel& item) const noexcept = default;

    protected:
        context_id_array _context_ids;
    };
}
