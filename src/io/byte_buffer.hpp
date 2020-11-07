#ifndef GROWER_CONTROLLER_RPI_BYTE_BUFFER_HPP
#define GROWER_CONTROLLER_RPI_BYTE_BUFFER_HPP

#include <vector>
#include <cstdint>

namespace grower::io {
    class little_endian_trait {
    public:
        static void add_to_buffer(const void *value, std::size_t num_bytes, std::vector<uint8_t> &buffer);
    };

    class big_endian_trait {
    public:
        static void add_to_buffer(const void *value, std::size_t num_bytes, std::vector<uint8_t> &buffer);
    };

    class byte_buffer {
        std::vector<uint8_t> _buffer{};

    public:
        template<typename type, typename endian_trait = little_endian_trait>
        byte_buffer &append(const type &value) {
            endian_trait::add_to_buffer(&value, sizeof(type), _buffer);
            return *this;
        }

        byte_buffer &append(const std::vector<uint8_t> &value) {
            _buffer.insert(_buffer.end(), value.begin(), value.end());
            return *this;
        }

        [[nodiscard]] const std::vector<uint8_t> &buffer() const {
            return _buffer;
        }
    };
}

#endif //GROWER_CONTROLLER_RPI_BYTE_BUFFER_HPP
