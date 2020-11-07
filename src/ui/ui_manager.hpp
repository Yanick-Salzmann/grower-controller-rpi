#ifndef GROWER_CONTROLLER_RPI_UI_MANAGER_HPP
#define GROWER_CONTROLLER_RPI_UI_MANAGER_HPP

#include "tslib.h"
#include <list>
#include <vector>

#include "utils/singleton.hpp"
#include "text/text_sprite.hpp"
#include "utils/log.hpp"
#include "touch_input.hpp"
#include "views/view.hpp"
#include "views/main_view.hpp"

namespace grower::ui {
    class ui_manager : public utils::singleton<ui_manager> {
        LOGGER;

        bool _is_display_on = true;
        std::size_t _frame_count = 0;

        std::chrono::steady_clock::time_point _last_fps_update = std::chrono::steady_clock::now();
        std::chrono::steady_clock::time_point _last_temp_update = std::chrono::steady_clock::now() - std::chrono::seconds{10};
        std::chrono::steady_clock::time_point _last_input_event = std::chrono::steady_clock::now();

        std::shared_ptr<text::text_sprite> _fps_indicator;
        std::shared_ptr<text::text_sprite> _temp_indicator;

        std::unique_ptr<touch_input> _touch_input{};

        std::vector<views::view_ptr> _views{};
        views::view_ptr _active_view{};

        std::shared_ptr<views::main_view> _main_view{};

        void update_fps();
        void update_temperature();

        void turn_off_display();
        void turn_on_display();

    public:
        void initialize();

        [[nodiscard]] bool should_quit() const {
            return false;
        }

        void render();

        void handle_input_event(const input_event& event);

        static const std::string default_font_name;
        static const std::string font_awesome_name;
    };
}

#define s_ui_mgr (grower::ui::ui_manager::instance())

#endif //GROWER_CONTROLLER_RPI_UI_MANAGER_HPP
