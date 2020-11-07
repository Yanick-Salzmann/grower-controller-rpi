#ifndef GROWER_CONTROLLER_RPI_CASE_FAN_HPP
#define GROWER_CONTROLLER_RPI_CASE_FAN_HPP

#include <chrono>
#include <thread>
#include <atomic>
#include "wiring/gpio.hpp"

namespace grower::peripheral {
    class case_fan {
        wiring::gpio_pin _pwm_pin;
        wiring::gpio_pin _rpm_pin;

        std::size_t _rpm = 0;
        std::size_t _speed = 0;

        std::thread _rpm_update_thread;
        std::atomic_bool _rpm_running = true;

        void rpm_update_callback();

    public:
        case_fan(int pwm_pin_index, int rpm_pin_index, std::size_t initial_speed = 0);
        ~case_fan();

        case_fan(const case_fan&) = delete;
        case_fan(case_fan&&) = delete;

        void operator = (const case_fan&) = delete;
        void operator = (case_fan&&) = delete;

        case_fan& speed(std::size_t new_speed);

        [[nodiscard]] std::size_t speed() const {
            return _speed;
        }

        [[nodiscard]] std::size_t rpm() const {
            return _rpm;
        }
    };
}

#endif //GROWER_CONTROLLER_RPI_CASE_FAN_HPP
