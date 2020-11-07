#ifndef GROWER_CONTROLLER_RPI_ICON_BUTTON_HPP
#define GROWER_CONTROLLER_RPI_ICON_BUTTON_HPP

#include "button.hpp"
#include "ui/text/font-awesome-icons.hpp"

namespace grower::ui::elements {
    class icon_button : public button {
    protected:
        void update_text(const std::string &text) override;
        void update_dimensions() override;

        void init();

    public:
        icon_button() { init(); }
        explicit icon_button(const std::string& text) : button(text) { init(); }
        explicit icon_button(glm::vec2 position) : button(position) { init(); }
        icon_button(float x, float y) : button(x, y) { init(); }
        icon_button(text::font_awesome icon, glm::vec2 position) : button(text::font_awesome_code(icon), position) { init(); }
        icon_button(text::font_awesome icon, float x, float y) : button(text::font_awesome_code(icon), x, y) { init(); }
    };
}

#endif //GROWER_CONTROLLER_RPI_ICON_BUTTON_HPP
