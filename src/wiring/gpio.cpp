#include "gpio.hpp"
#include <wiringPi.h>
#include <algorithm>

namespace grower::wiring {
    gpio_pin& gpio_pin::mode(gpio_mode mode) {
        switch(mode) {
            case gpio_mode::output:
                pinMode(_pin_index, OUTPUT);
                break;

            case gpio_mode::input:
                pinMode(_pin_index, INPUT);
                break;

            case gpio_mode::pwm:
                pinMode(_pin_index, PWM_OUTPUT);
                break;

            case gpio_mode::pull_down:
                pullUpDnControl(_pin_index, PUD_DOWN);
                pinMode(_pin_index, INPUT);
                break;

            case gpio_mode::pull_up:
                pinMode(_pin_index, INPUT);
                pullUpDnControl(_pin_index, PUD_UP);
                break;
        }

        return *this;
    }

    gpio_pin& gpio_pin::high() {
        digitalWrite(_pin_index, HIGH);
        return *this;
    }

    gpio_pin& gpio_pin::low() {
        digitalWrite(_pin_index, LOW);
        return *this;
    }

    bool gpio_pin::state() const {
        return digitalRead(_pin_index) == HIGH;
    }

    uint8_t gpio_pin::read() {
        return digitalRead(_pin_index);
    }

    gpio_pin &gpio_pin::pwm_value(int value) {
        value = std::min(std::max(value, 0), 1023);
        pwmWrite(_pin_index, value);
        return *this;
    }
}