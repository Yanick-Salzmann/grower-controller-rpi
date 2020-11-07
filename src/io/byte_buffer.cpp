#include "byte_buffer.hpp"

namespace grower::io {

    void little_endian_trait::add_to_buffer(const void *value, std::size_t num_bytes, std::vector<uint8_t> &buffer) {
        const auto ptr = (const uint8_t *) value;
        buffer.insert(buffer.end(), ptr, ptr + num_bytes);
    }

    void big_endian_trait::add_to_buffer(const void *value, std::size_t num_bytes, std::vector<uint8_t> &buffer) {
        const auto ptr = (const uint8_t *) value;
        buffer.insert(buffer.end(), std::make_reverse_iterator(ptr + num_bytes), std::make_reverse_iterator(ptr));
    }
}