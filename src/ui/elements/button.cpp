#include "button.hpp"
#include "ui/ui_manager.hpp"

namespace grower::ui::elements {

    button::button() : _caption{ui_manager::default_font_name} {
        _background.size(150, 50)
                .color(0xFFFF7F3F);
        _caption.font_size(30.0f);
        _border_top.position(-1.0f, -1.0f)
                .size(152.0f, 1.0f)
                .color(0xFFF8F8FF);
        _border_bottom.position(-1.0f, 50.0f)
                .size(152.0f, 1.0f)
                .color(0xFFF8F8FF);
        _border_left.position(-1.0f, -1.0f)
                .size(1.0f, 52.0f)
                .color(0xFFF8F8FF);
        _border_right.position(150.0f, -1.0f)
                .size(1.0f, 52.0f)
                .color(0xFFF8F8FF);
    }

    button::button(const std::string &text) : button() {
        update_text(text);
    }

    button::button(glm::vec2 position) : button() {
        _background.position(position);
    }

    button::button(float x, float y) : button(glm::vec2{x, y}) {

    }

    button::button(const std::string &text, glm::vec2 position) : button(position) {
        update_text(text);
    }

    button::button(const std::string &text, float x, float y) : button(text, {x, y}) {

    }

    void button::render() {
        _background.render();
        _border_left.render();
        _border_right.render();
        _border_top.render();
        _border_bottom.render();

        if (!_text.empty()) {
            _caption.render();
        }
    }

    void button::update_text(const std::string &text) {
        _text = text;
        _caption.text(text);
        _text_size = _caption.dimension();
        update_dimensions();
    }

    void button::update_dimensions() {
        const auto width = _text.empty() ? 150.0f : std::max(150.0f, _text_size.x + 30);
        if (!_text.empty()) {
            _caption.position(_background.position().x + 15, _background.position().y + (50 - _text_size.y) / 2.0f);
        }
        _background.size(width, 50.0f);
        update_border();
    }

    button &button::position(glm::vec2 position) {
        _background.position(position);
        update_dimensions();
        return *this;
    }

    void button::update_border() {
        _border_top.position(_background.position().x - 1.0f, _background.position().y - 1.0f)
                .size(_background.size().x + 2.0f, 1.0f);
        _border_left.position(_background.position().x - 1.0f, _background.position().y - 1.0f)
                .size(1.0f, _background.size().y + 2.0f);
        _border_bottom.position(_background.position().x - 1.0f, _background.position().y + _background.size().y)
                .size(_background.size().x + 2.0f, 1.0f);
        _border_right.position(_background.position().x + _background.size().x, _background.position().y - 1.0f)
                .size(1.0f, _background.size().y + 2.0f);
    }

    glm::vec2 button::position() {
        return _background.position();
    }

    glm::vec2 button::size() {
        return _background.size();
    }

    void button::touch_down(const input_event &e) {
        if (e.x >= position().x && e.x <= position().x + size().x &&
            e.y >= position().y && e.y <= position().y + size().y) {
            _background.color(0xFF3F7FFF);
            _is_pressed = true;
        }

        _is_touch_up = false;
    }

    void button::touch_up(const input_event &e) {
        _background.color(0xFFFF7F3F);
        if (e.x >= position().x && e.x <= position().x + size().x &&
            e.y >= position().y && e.y <= position().y + size().y &&
            _is_pressed && _click_callback) {
            _click_callback();
        }

        _is_pressed = false;
        _is_touch_up = true;
    }

    void button::touch_move(const input_event &e) {
        if (_is_touch_up) {
            return;
        }

        const auto is_hovered = e.x >= position().x && e.x <= position().x + size().x &&
                                e.y >= position().y && e.y <= position().y + size().y;

        _background.color(is_hovered ? 0xFF3F7FFF : 0xFFFF7F3F);
    }
}