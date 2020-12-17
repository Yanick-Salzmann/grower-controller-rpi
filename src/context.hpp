#ifndef GROWER_CONTROLLER_RPI_CONTEXT_HPP
#define GROWER_CONTROLLER_RPI_CONTEXT_HPP

#include <memory>
#include <peripheral/power_supply.hpp>
#include <peripheral/case_fan.hpp>
#include <peripheral/relay_control.hpp>
#include <peripheral/arduino_interface.hpp>
#include <peripheral/acs712.hpp>
#include <functional>
#include "utils/log.hpp"

namespace grower {
    enum class fan_type : std::size_t {
        cooler = 1,
        humidifier = 2,
        intake = 4,
        all = cooler | humidifier | intake
    };

    class context {
        LOGGER;

        std::shared_ptr<peripheral::case_fan> _cooler_fan{};
        peripheral::relay_control_ptr _germ_lamp{};
        peripheral::relay_control_ptr _flower_lamp{};
        peripheral::relay_control_ptr _cam_light{};

        peripheral::arduino_interface_ptr _arduino{};
        peripheral::acs712_ptr _lamp_meter{};
        peripheral::acs712_ptr _rpi_meter{};

        std::string find_serial_interface();

    public:
        void setup();

        [[nodiscard]] std::shared_ptr<peripheral::case_fan> cooler_fan() const {
            return _cooler_fan;
        }

        [[nodiscard]] peripheral::relay_control_ptr germ_lamp() const {
            return _germ_lamp;
        }

        [[nodiscard]] peripheral::relay_control_ptr flower_lamp() const {
            return _flower_lamp;
        }

        [[nodiscard]] peripheral::relay_control_ptr cam_light() const {
            return _cam_light;
        }

        [[nodiscard]] peripheral::arduino_interface_ptr arduino() const {
            return _arduino;
        }

        float lamp_current();

        float rpi_current();
    };

    typedef std::shared_ptr<context> context_ptr;
}

#endif //GROWER_CONTROLLER_RPI_CONTEXT_HPP
