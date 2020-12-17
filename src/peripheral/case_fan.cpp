#include "case_fan.hpp"
#include <algorithm>
#include "utils/literals.hpp"

namespace grower::peripheral {

    case_fan::case_fan(int pwm_pin_index, int rpm_pin_index, std::size_t initial_speed) :
            _pwm_pin{wiring::gpio::open_pin(pwm_pin_index)},
            _rpm_pin{wiring::gpio::open_pin(rpm_pin_index)} {
        _pwm_pin.pwm();
        speed(initial_speed);

        _rpm_pin.pull_up();

        _rpm_update_thread = std::thread{[this]() { rpm_update_callback(); }};
    }

    case_fan &case_fan::speed(std::size_t new_speed) {
        new_speed = std::min(std::max(new_speed, 0_sz), 255_sz);
        _pwm_pin.pwm_value(static_cast<int>(new_speed));
        return *this;
    }

    void case_fan::rpm_update_callback() {
        auto last_update = std::chrono::steady_clock::now();
        auto last_value = 1; // hall sensor pulls value to 0 when sensing a revolution

        auto num_revolutions = 0;

        while (_rpm_running) {
            const auto value = _rpm_pin.read();
            if (!value && last_value != 0) {
                ++num_revolutions;
            }

            last_value = value;

            auto now = std::chrono::steady_clock::now();
            auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_update).count();
            if (diff >= 1000) {
                static const std::size_t sensors_per_revolution = 2;

                _rpm = (std::size_t) (((((float) num_revolutions) / diff) * 1000.0f) * 60 / sensors_per_revolution);
                last_update = now;
                num_revolutions = 0;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds{1});
        }
    }

    case_fan::~case_fan() {
        _rpm_running = false;
        if (_rpm_update_thread.joinable()) {
            _rpm_update_thread.join();
        }
    }
}