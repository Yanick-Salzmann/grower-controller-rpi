#include "acs712.hpp"
#include <numeric>

namespace grower::peripheral {
    LOGGER_IMPL(acs712);

    acs712::acs712(arduino_interface_ptr arduino, uint32_t analog_pin, bool dc_mode) : _arduino{std::move(arduino)}, _pin{analog_pin}, _dc_mode{dc_mode} {
        _arduino->subscribe(_pin, [this](int32_t value) { handle_value(value); });
    }

    acs712::~acs712() {
        _arduino->unsubscribe(_pin);
    }

    void acs712::handle_value(int32_t value) {
        const auto now = std::chrono::steady_clock::now();
        const auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - _last_update).count();
        if (_dc_mode) {
            _values.emplace_back((value - 512.0f) / 1024.0f);
            if (diff >= 1000) {
                const auto avg = std::accumulate(_values.begin(), _values.end(), 0.0f, [](const auto &cur, const auto &next) { return cur + next; })
                                 / _values.size();
                _values.clear();

                _last_value = (avg * 2500.0f) / 185;
                _last_update = now;

            }
        } else {
            _values.emplace_back(std::pow(value - 512.0f, 2.0));
            if (diff >= 1000) {
                const auto n = _values.size();
                const auto sum = std::accumulate(_values.begin(), _values.end(), 0.0f, [&n](const auto &cur, const auto &next) { return cur + next / (float) n; });
                _values.clear();

                const auto rms = std::sqrt(sum);
                _last_value = ((rms / 1024.0f) * 2500.0f) / 185;
                _last_update = now;
            }
        }
    }
}