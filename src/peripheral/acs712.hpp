#ifndef GROWER_CONTROLLER_RPI_ACS712_HPP
#define GROWER_CONTROLLER_RPI_ACS712_HPP

#include "arduino_interface.hpp"
#include "utils/log.hpp"

namespace grower::peripheral {
    class acs712 {
        LOGGER;

        bool _dc_mode;
        arduino_interface_ptr _arduino;
        uint32_t _pin;
        std::chrono::steady_clock::time_point _last_update = std::chrono::steady_clock::now();
        std::vector<float> _values{};

        float _last_value = 0.0f;

        void handle_value(int32_t value);

    public:
        acs712(arduino_interface_ptr arduino, uint32_t analog_pin, bool dc_mode);
        ~acs712();

        [[nodiscard]] float current() const {
            return _last_value;
        }
    };

    typedef std::shared_ptr<acs712> acs712_ptr;
}

#endif //GROWER_CONTROLLER_RPI_ACS712_HPP
