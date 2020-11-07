#include "view.hpp"

namespace grower::ui::views {

    void view::handle_input_message(const input_event &event) {
        if (event.type == input_event_type::touch_release) {
            for (const auto &element : _elements) {
                element->touch_up(event);
            }
        } else if (event.type == input_event_type::touch_press) {
            for (const auto &element : _elements) {
                const auto &pos = element->position();
                const auto &size = element->size();

                if (pos.x <= event.x && pos.x + size.x >= event.x &&
                    pos.y <= event.y && pos.y + size.y >= event.y) {
                    event.type == input_event_type::touch_press ? element->touch_down(event) : element->touch_up(event);
                } else {
                    element->touch_up(event);
                }
            }
        } else if (event.type == input_event_type::touch_move) {
            for (const auto &element : _elements) {
                element->touch_move(event);
            }
        }
    }

    void view::render() {
        for(auto& element : _elements) {
            element->render();
        }
    }
}