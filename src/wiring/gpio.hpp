#ifndef GROWER_CONTROLLER_RPI_GPIO_HPP
#define GROWER_CONTROLLER_RPI_GPIO_HPP

#include <cstdint>

namespace grower::wiring {
    enum class gpio_mode {
        output,
        input,
        pwm,
        pull_up,
        pull_down
    };

    class gpio_pin {
        gpio_mode  _mode = gpio_mode::input;
        uint32_t _pin_index = 0;

    public:
        explicit gpio_pin(uint32_t index) : _pin_index{index} {

        }

        gpio_pin& mode(gpio_mode mode);

        gpio_pin& input() {
            mode(gpio_mode::input);
            return *this;
        }

        gpio_pin& output() {
            mode(gpio_mode::output);
            return *this;
        }

        gpio_pin& pwm() {
            mode(gpio_mode::pwm);
            return *this;
        }

        gpio_pin& pull_up() {
            mode(gpio_mode::pull_up);
            return *this;
        }

        gpio_pin& pull_down() {
            mode(gpio_mode::pull_down);
            return *this;
        }

        [[nodiscard]] gpio_mode mode() const {
            return _mode;
        }

        gpio_pin& high();
        gpio_pin& low();

        [[nodiscard]] bool state() const;

        uint8_t read();

        gpio_pin& pwm_value(int value);
    };

    class gpio {
    public:
        static gpio_pin open_pin(uint32_t index) {
            return gpio_pin{index};
        }
    };
}

#endif //GROWER_CONTROLLER_RPI_GPIO_HPP
