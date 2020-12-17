#include "relay_control.hpp"

namespace grower::peripheral {

    relay_control::relay_control(uint32_t pin_index, bool initial_state) : _control_pin(wiring::gpio::open_pin(pin_index)) {
        _control_pin.output();
        state(initial_state);
    }

    void relay_control::state(bool state) {
        state ? _control_pin.low() : _control_pin.high();
    }

    bool relay_control::current_state() const {
        return !_control_pin.state();
    }
}