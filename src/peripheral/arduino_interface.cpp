#include "arduino_interface.hpp"
#include "utils/string.hpp"

namespace grower::peripheral {
    LOGGER_IMPL(arduino_interface);

    arduino_interface::arduino_interface(const std::string &serial_tty) : _serial{serial_tty, 115200} {
        log->info("Opening serial interface to Arduino on {} @ 115200bps", serial_tty);
        _read_thread = std::thread{[this]() { serial_read_thread(); }};
    }

    void arduino_interface::shutdown() {
        if (!_is_running || !_read_thread.joinable()) {
            return;
        }

        _is_running = false;
        _read_thread.join();
    }

    void arduino_interface::serial_read_thread() {
        for (auto i = 0; i < MAX_ANALOG_INPUTS * 2; ++i) {
            _serial.read_line();
        }

        while (_is_running) {
            auto line = _serial.read_line();

            const auto command = static_cast<serial_input_command>(line[0]);
            line = line.substr(1);
            switch (command) {
                case serial_input_command::current_value:
                    handle_current_update(line);
                    break;

                case serial_input_command::digital_read_value:
                    handle_digital_output(line);
                    break;

                case serial_input_command::analog_read_value:
                    handle_analog_output(line);
                    break;
            }
        }
    }

    void arduino_interface::handle_current_update(const std::string &line) {
        auto parts = utils::split(line, ' ');
        if (parts.size() < 2) {
            return;
        }

        auto index = std::stoi(parts[0]);
        auto value = std::stoi(parts[parts.size() - 1]);

        if (_callbacks[index]) {
            _callbacks[index](value);
        }
    }

    void arduino_interface::handle_digital_output(const std::string &line) {
        const auto pin = (uint8_t) line[0];
        auto req_id = (uint32_t) line[1];
        req_id |= ((uint32_t) line[2]) << 8u;

        const auto state = line[3] != 0;
        std::function<void(uint32_t, bool)> callback{};
        {
            std::lock_guard<std::mutex> l{_callback_lock};
            const auto itr = _digital_callbacks.find(req_id);
            if (itr != _digital_callbacks.end()) {
                callback = itr->second;
                _digital_callbacks.erase(itr);
            }
        }

        if (callback) {
            callback(pin, state);
        }
    }

    void arduino_interface::handle_analog_output(const std::string &line) {
        const auto pin = (uint8_t) line[0];
        auto req_id = (uint32_t) line[1];
        req_id |= ((uint32_t) line[2]) << 8u;

        auto state = (uint32_t) line[3];
        state |= ((uint32_t) line[4]) << 8u;

        std::function<void(uint32_t, uint32_t)> callback{};
        {
            std::lock_guard<std::mutex> l{_callback_lock};
            const auto itr = _analog_callbacks.find(req_id);
            if (itr != _analog_callbacks.end()) {
                callback = itr->second;
                _analog_callbacks.erase(itr);
            }
        }

        if (callback) {
            callback(pin, state);
        }
    }

    void arduino_interface::write_servo_angle(std::size_t servo_index, uint32_t angle) {
        char cmd[] = {
                (char) serial_output_commands::servo_set_angle,
                (char) servo_index,
                std::min<char>(std::max<char>(angle, 20), 160),
                0
        };

        _serial.write({std::begin(cmd), std::end(cmd)});
    }

    void arduino_interface::write_analog_value(std::size_t analog_pin, uint32_t value) {
        char cmd[] = {
                (char) serial_output_commands::analog_set_value,
                (char) analog_pin,
                (char) (value & 0xFFu),
                0
        };

        _serial.write({std::begin(cmd), std::end(cmd)});
    }

    void arduino_interface::write_digital_value(std::size_t pin, bool state) {
        char cmd[] = {
                (char) serial_output_commands::digital_set_value,
                (char) pin,
                (char) (state ? 1 : 0),
                0
        };

        _serial.write({std::begin(cmd), std::end(cmd)});
    }
}