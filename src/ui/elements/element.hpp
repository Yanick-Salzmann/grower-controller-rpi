#ifndef GROWER_CONTROLLER_RPI_ELEMENT_HPP
#define GROWER_CONTROLLER_RPI_ELEMENT_HPP

#include <memory>
#include "glm/glm.hpp"
#include "ui/touch_input.hpp"

namespace grower::ui::elements {
    class element {
    public:
        virtual glm::vec2 position() = 0;
        virtual glm::vec2 size() = 0;

        virtual void render() = 0;

        virtual void touch_down(const input_event& e) {

        }

        virtual void touch_up(const input_event& e) {

        }

        virtual void touch_move(const input_event& e) {

        }
    };

    typedef std::shared_ptr<element> element_ptr;
}

#endif //GROWER_CONTROLLER_RPI_ELEMENT_HPP
