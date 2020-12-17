#include "slider.hpp"
#include "utils/colors.hpp"

namespace grower::ui::elements {
    slider::slider() {
        update_size_position();

        _left_corner.radius(10.0f)
                .color(utils::colors::cornflower_blue);
        _right_corner.radius(10.0f)
                .color(utils::colors::cornflower_blue);

        _middle_bar.color(utils::colors::cornflower_blue);
    }

    glm::vec2 slider::position() {
        return _position;
    }

    glm::vec2 slider::size() {
        return _size;
    }

    void slider::render() {
        _left_corner.render();
        _right_corner.render();
        _middle_bar.render();
    }

    void slider::update_size_position() {
        _left_corner.position(_position.x + 10.0f, _position.y + 17.5f);
        _right_corner.position(_position.x + _size.x - 10.0f, _position.y + 17.5f);


        _middle_bar.position(_position.x + 10.0f, _position.y + 7.5f)
                .size(_size.x - 20.0f, 20.0f);
    }
}