#include "pigpio_library.hpp"
#include <utils/log.hpp>
#include "pigpiod_if2.h"

namespace grower::peripheral {
    utils::logger log{"grower::peripheral::pigpiod"};

    int32_t pigpiod_handle() {
        static std::once_flag init_flag{};
        static int32_t handle = -1;

        std::call_once(init_flag, []() {
            int32_t result = pigpio_start(nullptr, nullptr);
            if (result < 0) {
                log->warn("Error opening pigpio deamon: {}", result);
            }

            handle = result;
        });

        return handle;
    }
}