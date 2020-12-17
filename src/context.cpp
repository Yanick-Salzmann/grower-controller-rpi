#include <utils/error.hpp>
#include "context.hpp"

namespace grower {
    LOGGER_IMPL(context);

    void context::setup() {
        _cooler_fan = std::make_shared<peripheral::case_fan>(19, 21);
        _cooler_fan->speed(255);

        _germ_lamp = std::make_shared<peripheral::relay_control>(13, false);
        _flower_lamp = std::make_shared<peripheral::relay_control>(22, false);
        _cam_light = std::make_shared<peripheral::relay_control>(17, false);

        const auto serial_iface = find_serial_interface();
        if(serial_iface.empty()) {
            throw std::runtime_error{"Error finding arduino USB serial interface"};
        }

        _arduino = std::make_shared<peripheral::arduino_interface>(serial_iface);

        _lamp_meter = std::make_shared<peripheral::acs712>(_arduino, 5, false);
        _rpi_meter = std::make_shared<peripheral::acs712>(_arduino, 4, true);
    }

    float context::lamp_current() {
        if (!_germ_lamp->state() && !_flower_lamp->state()) {
            return 0.0f;
        }

        return _lamp_meter->current();
    }

    float context::rpi_current() {
        return _rpi_meter->current();
    }

    std::string context::find_serial_interface() {
        for(auto i = 0; i < 4; ++i) {
            const auto iface = fmt::format("/dev/ttyUSB{}", i);
            const auto fd = open(iface.c_str(), O_RDONLY);
            if(fd >= 0) {
                close(fd);
                return iface;
            } else {
                log->debug("Skipping {}: {}", iface, utils::error_to_string(errno));
            }
        }

        return {};
    }
}