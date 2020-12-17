#include "checkbox.hpp"
#include "ui/ui_manager.hpp"

namespace grower::ui::elements {

    void checkbox::render() {
        if (_is_checked) {
            _checked_image->render();
        } else {
            _unchecked_image->render();
        }

        _caption->render();
    }

    checkbox::checkbox() {
        _checked_image = std::make_shared<font_awesome_image>(text::font_awesome::check_square);
        _checked_image->size(CHECKBOX_SIZE);
        _unchecked_image = std::make_shared<font_awesome_image>(text::font_awesome::square);
        _unchecked_image->size(CHECKBOX_SIZE);

        _caption = std::make_shared<text::text_sprite>(ui_manager::default_font_name);
        _caption->position(CHECKBOX_SIZE + CHECKBOX_TEXT_OFFSET, 0.0f).font_size(CHECKBOX_FONT_SIZE);
    }

    glm::vec2 checkbox::position() {
        return _position;
    }

    glm::vec2 checkbox::size() {
        return _size;
    }

    void checkbox::update_positions() {
        _caption->position(_position.x + CHECKBOX_SIZE + CHECKBOX_TEXT_OFFSET, _position.y + (CHECKBOX_SIZE - _caption->size().y) / 2.0f);
        _checked_image->position(_position);
        _unchecked_image->position(_position);
        _size = {CHECKBOX_SIZE + CHECKBOX_TEXT_OFFSET + _caption->size().x, CHECKBOX_SIZE};
    }

    checkbox &checkbox::position(const glm::vec2 &pos) {
        _position = pos;
        update_positions();
        return *this;
    }

    void checkbox::touch_down(const input_event &e) {
        if (e.x >= position().x && e.x <= position().x + size().x &&
            e.y >= position().y && e.y <= position().y + size().y) {
            _is_pressed = true;
        }
    }

    void checkbox::touch_up(const input_event &e) {
        if (e.x >= position().x && e.x <= position().x + size().x &&
            e.y >= position().y && e.y <= position().y + size().y &&
            _is_pressed) {
            _is_checked = !_is_checked;
            if (_callback) {
                _callback(_is_checked);
            }
        }

        _is_pressed = false;
    }
}