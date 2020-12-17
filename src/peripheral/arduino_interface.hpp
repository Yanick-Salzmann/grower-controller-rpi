#ifndef GROWER_CONTROLLER_RPI_ARDUINO_INTERFACE_HPP
#define GROWER_CONTROLLER_RPI_ARDUINO_INTERFACE_HPP

#include <string>
#include "serial_interface.hpp"
#include <memory>
#include <map>
#include "utils/log.hpp"

namespace grower::peripheral {
    enum class serial_input_command {
        analog_read_value,
        digital_read_value,
        current_value
    };

    enum class serial_output_commands {
        pwm_set_value,
        digital_set_value,
        digital_read_value,
        digital_set_mode,
        analog_set_value,
        analog_read_value,
        servo_set_angle
    };

    class arduino_interface {
        LOGGER;

        static constexpr std::size_t MAX_ANALOG_INPUTS = 16;

        serial_interface _serial;
        std::thread _read_thread;
        volatile bool _is_running = true;

        std::array<std::function<void(int32_t)>, MAX_ANALOG_INPUTS> _callbacks{};

        std::atomic_uint16_t _req_counter{};
        std::mutex _callback_lock{};
        std::map<uint32_t, std::function<void(uint32_t, bool)>> _digital_callbacks{};
        std::map<uint32_t, std::function<void(uint32_t, uint32_t)>> _analog_callbacks{};

        void serial_read_thread();

        void handle_analog_output(const std::string& line);
        void handle_digital_output(const std::string& line);
        void handle_current_update(const std::string& line);

    public:
        explicit arduino_interface(const std::string &serial_tty = "/dev/ttyUSB0");

        ~arduino_interface() {
            shutdown();
        }

        void shutdown();

        uint32_t read_analog_value(std::size_t analog_poin);
        bool read_digital_value(std::size_t pin);

        void write_digital_value(std::size_t pin, bool state);
        void write_analog_value(std::size_t analog_pin, uint32_t value);
        void write_servo_angle(std::size_t servo_index, uint32_t angle);

        arduino_interface &subscribe(std::size_t analog_pin, std::function<void(int32_t)> callback) {
            if (analog_pin >= MAX_ANALOG_INPUTS) {
                throw std::out_of_range{"Index out of range"};
            }

            _callbacks[analog_pin] = std::move(callback);
            return *this;
        }

        arduino_interface &unsubscribe(std::size_t analog_pin) {
            if (analog_pin >= MAX_ANALOG_INPUTS) {
                throw std::out_of_range{"Index out of range"};
            }

            _callbacks[analog_pin] = {};
            return *this;
        }
    };

    typedef std::shared_ptr<arduino_interface> arduino_interface_ptr;
}

#endif //GROWER_CONTROLLER_RPI_ARDUINO_INTERFACE_HPP
