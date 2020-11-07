#include "context.hpp"

namespace grower {
    void context::setup() {
        _psu = std::make_shared<peripheral::power_supply>(11);
        _cooler_fan = std::make_shared<peripheral::case_fan>(12, 13);

        _psu->activate();
        _cooler_fan->speed(0);
    }

    context::~context() {
        _psu->deactivate();
    }
}