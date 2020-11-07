#include "icon_button.hpp"
#include "ui/ui_manager.hpp"

namespace grower::ui::elements {
    void icon_button::update_text(const std::string &text) {
        _text = text;
        _caption.text(text);

        update_dimensions();
    }

    void icon_button::update_dimensions() {
        update_border();

        const auto width = _text.empty() ? 0.0f : _caption.dimension().x;
        const auto height = _text.empty() ? 0.0f : _caption.dimension().y;

        _caption.position(
                _background.position().x + (_background.size().x - width) / 2.0f,
                _background.position().y + (_background.size().y - height) / 2.0f
        );
    }

    void icon_button::init() {
        _caption = text::text_sprite(ui_manager::font_awesome_name);
        _caption.font_size(60.0f);

        if (!_text.empty()) {
            _caption.text(_text);
        }

        _background.size(100, 100);

        update_dimensions();
    }
}