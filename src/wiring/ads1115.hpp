#ifndef GROWER_CONTROLLER_RPI_ADS1115_HPP
#define GROWER_CONTROLLER_RPI_ADS1115_HPP

#include <cstdint>

namespace grower::wiring {
    class ads1115 {
        int _device;
        uint16_t _gain = 0;
        uint16_t _sps = 0;
        uint32_t _conversion_delay = 0;
    public:
        explicit ads1115(int device_id = 0x48);

        uint16_t read_single_channel(uint8_t channel);
    };
}

#endif //GROWER_CONTROLLER_RPI_ADS1115_HPP
