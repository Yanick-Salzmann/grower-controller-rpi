#include "main_view.hpp"
#include "ui/ui_manager.hpp"

namespace grower::ui::views {
    main_view::main_view() {
        _caption = std::make_shared<text::text_sprite>(ui_manager::default_font_name);
        _caption->text("Plant Growing Utility Control Panel")
                .font_size(50)
                .position(20.0f, 5.0f);

        _camera_frame = std::make_shared<camera_frame>();

        add_element(_caption);
        add_element(_camera_frame);
    }
}