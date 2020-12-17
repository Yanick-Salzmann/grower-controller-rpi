#ifndef GROWER_CONTROLLER_RPI_MAIN_VIEW_HPP
#define GROWER_CONTROLLER_RPI_MAIN_VIEW_HPP

#include "view.hpp"
#include "ui/text/text_sprite.hpp"
#include "ui/camera_frame.hpp"
#include "peripheral/ads1115.hpp"
#include "context.hpp"
#include "ui/elements/color_circle.hpp"
#include "ui/elements/slider.hpp"

namespace grower::ui::views {
    class main_view : public view {
        std::shared_ptr<text::text_sprite> _caption;
        std::shared_ptr<camera_frame> _camera_frame;
        std::shared_ptr<text::text_sprite> _lamp_power_caption;
        std::shared_ptr<text::text_sprite> _rpi_power_caption;

        std::shared_ptr<elements::slider> _slider;

        std::chrono::steady_clock::time_point _last_power_update = std::chrono::steady_clock::now();

        context_ptr _context;

    public:
        explicit main_view(context_ptr context);

        void render() override;
    };
}

#endif //GROWER_CONTROLLER_RPI_MAIN_VIEW_HPP
