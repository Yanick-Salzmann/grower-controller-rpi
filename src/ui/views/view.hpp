#ifndef GROWER_CONTROLLER_RPI_VIEW_HPP
#define GROWER_CONTROLLER_RPI_VIEW_HPP

#include "ui/touch_input.hpp"
#include "ui/elements/element.hpp"
#include <list>
#include <memory>

namespace grower::ui::views {
    class view {
    protected:
        std::list<elements::element_ptr> _elements{};

    public:
        virtual void render();
        virtual void handle_input_message(const input_event& event);

        void add_element(elements::element_ptr element) {
            _elements.emplace_back(std::move(element));
        }
    };

    typedef std::shared_ptr<view> view_ptr;
}

#endif //GROWER_CONTROLLER_RPI_VIEW_HPP
