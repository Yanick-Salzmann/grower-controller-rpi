#include "serial_interface.hpp"

#include "pigpio_library.hpp"
#include <pigpiod_if2.h>
#include <memory>
#include <memory.h>
#include <functional>

namespace grower::peripheral {
    LOGGER_IMPL(serial_interface);

    serial_interface::serial_interface(const std::string &device_name, uint32_t baud_rate) : _pig_handle{pigpiod_handle()} {
        std::unique_ptr<char, std::function<void(void *)>> ptr{strdup(device_name.c_str()), [](void *ptr) { free(ptr); }};

        const auto res = serial_open(_pig_handle, ptr.get(), baud_rate, 0);
        if (res < 0) {
            log->error("Error opening serial interface {} at {} bps: {}", device_name, baud_rate, res);
            throw std::runtime_error{"Error opening serial interface"};
        }

        _serial_handle = res;
    }

    std::string serial_interface::read_line() const {
        std::vector<char> bytes{};
        auto cur = serial_read_byte(_pig_handle, _serial_handle);
        while (cur != '\n') {
            if (cur == PI_SER_READ_NO_DATA) {
                std::this_thread::sleep_for(std::chrono::milliseconds{1});
                cur = serial_read_byte(_pig_handle, _serial_handle);
                continue;
            }

            bytes.emplace_back((char) cur);
            cur = serial_read_byte(_pig_handle, _serial_handle);
        }

        return std::string{bytes.begin(), bytes.end()};
    }

    void serial_interface::write(const std::string &data) {
        serial_write(_pig_handle, _serial_handle, const_cast<char*>(data.data()), data.size());
    }
}