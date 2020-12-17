#ifndef GROWER_CONTROLLER_RPI_TOUCH_INPUT_HPP
#define GROWER_CONTROLLER_RPI_TOUCH_INPUT_HPP

#include "tslib.h"
#include "utils/log.hpp"
#include <array>

namespace grower::ui {
    enum class input_event_type {
        touch_press,
        touch_release,
        touch_move,
        unknown
    };

    struct input_event {
        int32_t slot = -1;
        input_event_type type = input_event_type::unknown;

        int32_t x = -1;
        int32_t y = -1;
    };

    class touch_input {
        LOGGER;

        struct slot_state {
            bool pressed = false;
            int32_t x = 0;
            int32_t y = 0;
        };

        static constexpr std::size_t num_samples = 50;
        static constexpr std::size_t num_slots = 5;

        std::array<slot_state, num_slots> _slot_states{};

        ts_sample_mt** _sample_buffer = nullptr;

        tsdev *_ts_dev = nullptr;

        bool _has_read_data = false;

        void update_slot(uint32_t slot, int32_t x, int32_t y, bool pressed);

        void process_sample(const ts_sample_mt* samples);

        void fire_touch_event(uint32_t slot, input_event_type type);
        void fire_touch_state_change_event(uint32_t slot);
        void fire_touch_move_event(uint32_t slot);

    public:
        touch_input();

        ~touch_input();

        touch_input(touch_input &) = delete;

        touch_input(touch_input &&) = delete;

        void operator=(touch_input &) = delete;

        void operator=(touch_input &&) = delete;

        void update();
    };
}

#endif //GROWER_CONTROLLER_RPI_TOUCH_INPUT_HPP
