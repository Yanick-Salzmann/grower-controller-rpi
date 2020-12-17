#ifndef GROWER_CONTROLLER_RPI_SLIDER_HPP
#define GROWER_CONTROLLER_RPI_SLIDER_HPP

#include "element.hpp"
#include "color_circle.hpp"
#include "color_quad.hpp"

namespace grower::ui::elements {
    class slider : public element {
        color_circle _left_corner;
        color_circle _right_corner;
        color_quad _middle_bar;

        glm::vec2 _position{};
        glm::vec2 _size{100.0f, 35.0f};

        void update_size_position();

    public:
        slider();

        glm::vec2 position() override;

        glm::vec2 size() override;

        void render() override;

        slider &size(float width) {
            _size.y = width;
            update_size_position();
            return *this;
        }

        slider &position(glm::vec2 position) {
            _position = position;
            update_size_position();
            return *this;
        }

        slider &position(float x, float y) {
            return position({x, y});
        }
    };
}

#endif //GROWER_CONTROLLER_RPI_SLIDER_HPP
