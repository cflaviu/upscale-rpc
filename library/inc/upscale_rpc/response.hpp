#pragma once
#ifndef PCH
    #include <upscale_rpc/basic.hpp>
#endif

namespace upscale_rpc::message
{
    using context_id_array = array::simple<context_id_t, 16u>;

    class response: public basic
    {
    public:
        using result_count_array = array::nibble<16u>;

        static constexpr std::uint8_t marker = 0x40u;

        response();

        const context_id_array& context_ids() const noexcept { return _context_ids; }
        void set_context_ids(context_id_array item) { _context_ids = std::move(item); }
        void set_context_id(const std::uint8_t index, context_id_t item) noexcept { _context_ids[index] = item; }

        const result_count_array& result_counts() const noexcept { return _result_counts; }
        void set_result_counts(result_count_array item) { _result_counts = std::move(item); }
        void set_result_count(const std::uint8_t index, std::uint8_t item) noexcept { _result_counts.set(index, item); }

        std::uint8_t total_result_count() const noexcept { return _total_result_count; }
        void set_total_result_count() noexcept;

        const std::uint8_t* total_result_count_ptr() const noexcept { return &_total_result_count; }

        std::size_t primary_storage_size() const noexcept override;
        std::size_t storage_size() const noexcept override;

        std::uint8_t* serialize_to(std::uint8_t* const s) const noexcept override;
        const std::uint8_t* deserialize_from(const std::uint8_t* const s) noexcept override;

        bool operator==(const response& other) const noexcept;
        bool operator!=(const response& other) const noexcept = default;

    protected:
        context_id_array _context_ids;
        result_count_array _result_counts;
        std::uint8_t _total_result_count {};
    };
}
