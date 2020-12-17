#include <ui/text/font.hpp>
#include <utils/error.hpp>
#include "ui_manager.hpp"
#include "io/gauges.hpp"
#include "text/font-awesome-icons.hpp"

namespace grower::ui {
    LOGGER_IMPL(ui_manager);

    const std::string ui_manager::default_font_name{"Titillium Web"};
    const std::string ui_manager::font_awesome_name{"Font Awesome 5 Free"};

    void ui_manager::initialize(context_ptr context) {
        _context = std::move(context);
        _touch_input = std::make_unique<touch_input>();

        ui::text::font::load_default_fonts();

        _temp_indicator = std::make_shared<text::text_sprite>(default_font_name);
        _temp_indicator->font_size(20);

        _fps_indicator = std::make_shared<text::text_sprite>(default_font_name);
        _fps_indicator->font_size(20);

        _main_view = std::make_shared<views::main_view>(_context);
        _active_view = _main_view;

        _views.push_back(_main_view);

        turn_on_display();
    }

    void ui_manager::render() {
        if (!_is_display_on) {
            std::this_thread::sleep_for(std::chrono::milliseconds{50});
        } else {
            const auto idle_time = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - _last_input_event).count();
            if (idle_time > 30) {
                log->info("Display has been idle for at least 30 seconds, going to standby");
                turn_off_display();
            }
        }

        _touch_input->update();

        if (_active_view) {
            _active_view->render();
        }

        ++_frame_count;

        update_fps();
        update_temperature();

        _fps_indicator->render();
        _temp_indicator->render();
    }

    void ui_manager::handle_input_event(const input_event &event) {
        _last_input_event = std::chrono::steady_clock::now();

        if (!_is_display_on) {
            log->info("Received touch event, waking up display");
            turn_on_display();
        }

        if (_active_view) {
            _active_view->handle_input_message(event);
        }
    }

    void ui_manager::update_temperature() {
        const auto now = std::chrono::steady_clock::now();
        const auto diff = std::chrono::duration_cast<std::chrono::seconds>(now - _last_temp_update).count();
        if (diff >= 3) {
            const auto temperature = io::read_temperature();
            if (temperature > 0.0f) {
                _temp_indicator->text(fmt::format("Temp: {}°C", (int32_t) temperature))
                        .position(1024.0f - 20.0f - _temp_indicator->dimension().x, 30.0f);
            }

            _last_temp_update = now;
        }
    }

    void ui_manager::update_fps() {
        const auto now = std::chrono::steady_clock::now();
        const auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(now - _last_fps_update).count();
        if (diff >= 1000) {
            const auto fps = (uint32_t) std::round(((_frame_count / (float) diff) * 1000.0f));
            _frame_count = 0;
            _last_fps_update = now;
            _fps_indicator->text(fmt::format("FPS: {}", fps))
                    .position(1024.0f - 20.0f - _fps_indicator->dimension().x, 5.0f);
        }
    }

    void ui_manager::turn_off_display() {
        const auto p = popen("vcgencmd display_power 0", "r");
        pclose(p);
        _is_display_on = false;
    }

    void ui_manager::turn_on_display() {
        const auto p = popen("vcgencmd display_power 1", "r");
        pclose(p);
        _is_display_on = true;
    }

    void ui_manager::update_display() {
        _touch_input->update();
    }
}