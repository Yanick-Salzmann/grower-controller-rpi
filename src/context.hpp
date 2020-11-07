#ifndef GROWER_CONTROLLER_RPI_CONTEXT_HPP
#define GROWER_CONTROLLER_RPI_CONTEXT_HPP

#include <memory>
#include <peripheral/power_supply.hpp>
#include <peripheral/case_fan.hpp>

namespace grower {
    enum class fan_type : std::size_t {
        cooler = 1,
        humidifier = 2,
        intake = 4,
        all = cooler | humidifier | intake
    };

    class context {
        std::shared_ptr<peripheral::power_supply> _psu{};
        std::shared_ptr<peripheral::case_fan> _cooler_fan{};

    public:
        ~context();

        void setup();

        [[nodiscard]] std::shared_ptr<peripheral::power_supply> psu() const {
            return _psu;
        }

        [[nodiscard]] std::shared_ptr<peripheral::case_fan> cooler_fan() const {
            return _cooler_fan;
        }
    };

    typedef std::shared_ptr<context> context_ptr;
}

#endif //GROWER_CONTROLLER_RPI_CONTEXT_HPP
