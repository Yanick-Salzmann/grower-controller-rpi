#include "power_supply.hpp"

namespace grower::peripheral {

    power_supply &power_supply::state(bool active) {
        active ? _trigger_pin.low() : _trigger_pin.high();
        return *this;
    }

    bool power_supply::state() const {
        return !_trigger_pin.state();
    }

    power_supply::power_supply(int relais_pin) : _trigger_pin{wiring::gpio::open_pin(relais_pin)} {
        _trigger_pin.output();
    }
}