#ifndef GROWER_CONTROLLER_RPI_RELAY_CONTROL_HPP
#define GROWER_CONTROLLER_RPI_RELAY_CONTROL_HPP

#include <memory>
#include "wiring/gpio.hpp"

namespace grower::peripheral {
    class relay_control {
        wiring::gpio_pin _control_pin;

        [[nodiscard]] bool current_state() const;

    public:
        explicit relay_control(uint32_t pin_index, bool initial_state = false);
        void state(bool state);

        [[nodiscard]] bool state() const {
            return current_state();
        }
    };

    typedef std::shared_ptr<relay_control> relay_control_ptr;
}

#endif //GROWER_CONTROLLER_RPI_RELAY_CONTROL_HPP
