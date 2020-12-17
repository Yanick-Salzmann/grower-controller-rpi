#include <utils/error.hpp>
#include "touch_input.hpp"
#include "ui_manager.hpp"

namespace grower::ui {
    LOGGER_IMPL(touch_input);

    touch_input::touch_input() {
        _sample_buffer = new ts_sample_mt *[num_samples];
        for (auto i = 0; i < num_samples; ++i) {
            _sample_buffer[i] = new ts_sample_mt[num_slots];
        }

        _ts_dev = ts_setup(nullptr, 1);
        if (!_ts_dev) {
            log->warn("Error opening touch device, input will not be available");
        }
    }

    touch_input::~touch_input() {
        if (_ts_dev) {
            ts_close(_ts_dev);
            _ts_dev = nullptr;
        }

        for (auto i = 0; i < num_samples; ++i) {
            delete[] _sample_buffer[i];
        }

        delete[] _sample_buffer;
    }

    void touch_input::update() {
        if (!_ts_dev) {
            return;
        }

        const auto num_read = ts_read_mt(_ts_dev, _sample_buffer, num_slots, num_samples);
        if (num_read > 0) {
            _has_read_data = true;
            for (auto i = 0; i < num_read; ++i) {
                process_sample(_sample_buffer[i]);
            }
        } else if (num_read < 0) {
            if (std::abs(num_read) != EAGAIN) {
                if(_has_read_data) {
                    log->warn("Error reading touch screen data: {}", utils::error_to_string(num_read));
                    _has_read_data = false;
                }
            }
        }
    }

    void touch_input::process_sample(const ts_sample_mt *samples) {
        for (auto i = 0; i < num_slots; ++i) {
            const auto &sample = samples[i];
#ifdef TSLIB_MT_VALID
            if (!(sample.valid & TSLIB_MT_VALID)) {
#else
                if(sample.valid < 1) {
#endif
                continue;
            }

            update_slot(sample.slot, sample.x, sample.y, sample.pressure > 0);
        }
    }

    void touch_input::update_slot(uint32_t slot, int32_t x, int32_t y, bool pressed) {
        if (slot >= _slot_states.size()) {
            return;
        }

        x = 1024 - x;
        y = 600 - y;

        auto &state = _slot_states[slot];
        const auto did_move = state.x != x || state.y != y;

        state.x = x;
        state.y = y;

        if (pressed != state.pressed) {
            state.pressed = pressed;
            fire_touch_state_change_event(slot);
        }

        if (did_move) {
            fire_touch_move_event(slot);
        }
    }

    void touch_input::fire_touch_state_change_event(uint32_t slot) {
        fire_touch_event(slot, _slot_states[slot].pressed ? input_event_type::touch_press : input_event_type::touch_release);
    }

    void touch_input::fire_touch_move_event(uint32_t slot) {
        fire_touch_event(slot, input_event_type::touch_move);
    }

    void touch_input::fire_touch_event(uint32_t slot, input_event_type type) {
        input_event event{};

        event.x = _slot_states[slot].x;
        event.y = _slot_states[slot].y;
        event.type = type;

        s_ui_mgr->handle_input_event(event);
    }
}