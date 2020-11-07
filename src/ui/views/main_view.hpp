#ifndef GROWER_CONTROLLER_RPI_MAIN_VIEW_HPP
#define GROWER_CONTROLLER_RPI_MAIN_VIEW_HPP

#include "view.hpp"
#include "ui/text/text_sprite.hpp"
#include "ui/camera_frame.hpp"

namespace grower::ui::views {
    class main_view : public view {
        std::shared_ptr<text::text_sprite> _caption;
        std::shared_ptr<camera_frame> _camera_frame;

    public:
        main_view();
    };
}

#endif //GROWER_CONTROLLER_RPI_MAIN_VIEW_HPP
