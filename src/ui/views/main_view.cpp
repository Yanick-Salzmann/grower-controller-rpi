#include "main_view.hpp"
#include "ui/ui_manager.hpp"
#include "ui/elements/checkbox.hpp"
#include "utils/colors.hpp"

namespace grower::ui::views {
    main_view::main_view(context_ptr context) : _context{std::move(context)} {
        _caption = std::make_shared<text::text_sprite>(ui_manager::default_font_name);
        _caption->text("Plant Growing Utility Control Panel")
                .font_size(50)
                .position(20.0f, 5.0f);

        _camera_frame = std::make_shared<camera_frame>();

        _lamp_power_caption = std::make_shared<text::text_sprite>(ui_manager::default_font_name);
        _lamp_power_caption->font_size(30.0f).position(20.0f, 100.0f);

        _rpi_power_caption = std::make_shared<text::text_sprite>(ui_manager::default_font_name);
        _rpi_power_caption->font_size(30.0f).position(20.0f, 150.0f);

        auto germ_cb = std::make_shared<elements::checkbox>();
        auto flower_cb = std::make_shared<elements::checkbox>();
        auto cam_cb = std::make_shared<elements::checkbox>();

        germ_cb->position(20.0f, 200.0f)
                .text("Germination Light")
                .checked(_context->germ_lamp()->state())
                .callback([this](bool checked) {
                    _context->germ_lamp()->state(checked);
                });

        flower_cb->position(20.0f, 200.0f + elements::checkbox::CHECKBOX_SIZE + 10.0f)
                .text("Flowering Light")
                .checked(_context->flower_lamp()->state())
                .callback([this](bool checked) {
                    _context->flower_lamp()->state(checked);
                });

        cam_cb->position(20.0f, 200.0f + 2 * (elements::checkbox::CHECKBOX_SIZE + 10.0f))
                .text("Camera Light")
                .checked(_context->cam_light()->state())
                .callback([this](bool checked) {
                    _context->cam_light()->state(checked);
                });

        _slider = std::make_shared<elements::slider>();
        _slider->position(20.0f, 400.0f);

        add_element(_caption);
        add_element(_camera_frame);
        add_element(_lamp_power_caption);
        add_element(_rpi_power_caption);
        add_element(germ_cb);
        add_element(flower_cb);
        add_element(cam_cb);
        add_element(_slider);
    }

    void main_view::render() {
        const auto now = std::chrono::steady_clock::now();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - _last_power_update).count() > 500) {
            _lamp_power_caption->text(fmt::format("Lamp Power: {:.{}f}W", (_context->lamp_current()) * 230.0f, 2));
            _rpi_power_caption->text(fmt::format("R-PI Power: {:.{}f}W", _context->rpi_current() * 5.0f, 2));
            _last_power_update = now;
        }

        view::render();
    }
}