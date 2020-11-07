#include "wiring_context.hpp"

#include <atomic>
#include <mutex>

#include <wiringPi.h>

namespace grower::wiring {
    wiring_context::wiring_context() {
        static std::once_flag _load_flag{};
        std::call_once(_load_flag, []() {
            if(wiringPiSetup() == -1) {
                throw std::runtime_error{"Error initializing wiringPi"};
            }

            wiringPiSetupPhys();
        });
    }
}