#include "gpio.hpp"
#include "pigpiod_if2.h"
#include "peripheral/pigpio_library.hpp"
#include <algorithm>

namespace grower::wiring {
    gpio_pin& gpio_pin::mode(gpio_mode mode) {
        switch(mode) {
            case gpio_mode::pwm:
                set_PWM_frequency(_pig_handle, _pin_index, 50);
            case gpio_mode::output:
                set_mode(_pig_handle, _pin_index, PI_OUTPUT);
                break;

            case gpio_mode::input:
                set_mode(_pig_handle, _pin_index, PI_INPUT);
                break;

            case gpio_mode::pull_down:
                set_pull_up_down(_pig_handle, _pin_index, PI_PUD_DOWN);
                set_mode(_pig_handle, _pin_index, PI_INPUT);
                break;

            case gpio_mode::pull_up:
                set_pull_up_down(_pig_handle, _pin_index, PI_PUD_UP);
                set_mode(_pig_handle, _pin_index, PI_INPUT);
                break;
        }

        return *this;
    }

    gpio_pin& gpio_pin::high() {
        gpio_write(_pig_handle, _pin_index, 1);
        return *this;
    }

    gpio_pin& gpio_pin::low() {
        gpio_write(_pig_handle, _pin_index, 0);
        return *this;
    }

    bool gpio_pin::state() const {
        return gpio_read(_pig_handle, _pin_index) != 0;
    }

    uint8_t gpio_pin::read() {
        return static_cast<uint8_t>(gpio_read(_pig_handle, _pin_index));
    }

    gpio_pin &gpio_pin::pwm_value(int value) {
        value = std::min(std::max(value, 0), 255);
        set_PWM_dutycycle(_pig_handle, _pin_index, value);
        return *this;
    }

    gpio_pin::gpio_pin(uint32_t index) : _pin_index{index} {
        _pig_handle = peripheral::pigpiod_handle();
    }
}