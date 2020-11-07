#ifndef GROWER_CONTROLLER_RPI_POWER_SUPPLY_HPP
#define GROWER_CONTROLLER_RPI_POWER_SUPPLY_HPP

#include "wiring/gpio.hpp"

namespace grower::peripheral {
    class power_supply {
        wiring::gpio_pin _trigger_pin;

    public:
        explicit power_supply(int relais_pin);

        power_supply& activate() {
            return state(true);
        }

        power_supply& deactivate() {
            return state(false);
        }

        power_supply& state(bool active);

        bool state() const;
    };
}

#endif //GROWER_CONTROLLER_RPI_POWER_SUPPLY_HPP
