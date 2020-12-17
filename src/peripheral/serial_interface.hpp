#ifndef GROWER_CONTROLLER_RPI_SERIAL_INTERFACE_HPP
#define GROWER_CONTROLLER_RPI_SERIAL_INTERFACE_HPP

#include <string>
#include <cstdint>
#include "utils/log.hpp"

namespace grower::peripheral {
    class serial_interface {
        LOGGER;

        int32_t _pig_handle{};
        int32_t _serial_handle{};

    public:
        serial_interface(const std::string& device_name, uint32_t baud_rate = 9600);

        std::string read_line() const;

        void write(const std::string& data);
    };
}

#endif //GROWER_CONTROLLER_RPI_SERIAL_INTERFACE_HPP
