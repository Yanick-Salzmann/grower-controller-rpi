#ifndef GROWER_CONTROLLER_RPI_BUTTON_HPP
#define GROWER_CONTROLLER_RPI_BUTTON_HPP

#include "color_quad.hpp"
#include "ui/text/text_sprite.hpp"
#include "element.hpp"

namespace grower::ui::elements {
    class button : public element {
    protected:
        color_quad _background;
        color_quad _border_top;
        color_quad _border_left;
        color_quad _border_right;
        color_quad _border_bottom;

        text::text_sprite _caption;
        bool _is_pressed = false;
        bool _is_touch_up = true;

        std::string _text{};
        glm::vec2 _text_size{};

        std::function<void()> _click_callback{};

        virtual void update_text(const std::string& text);
        virtual void update_dimensions();
        void update_border();

    public:
        button();
        explicit button(const std::string& text);
        explicit button(glm::vec2 position);
        button(float x, float y);
        button(const std::string& text, glm::vec2 position);
        button(const std::string& text, float x, float y);

        button& position(glm::vec2 position);

        void render() override;

        glm::vec2 position() override;

        glm::vec2 size() override;

        void touch_down(const input_event& e) override;

        void touch_up(const input_event& e) override;

        void touch_move(const input_event &e) override;

        button& click_callback(std::function<void()> callback) {
            _click_callback = std::move(callback);
            return *this;
        }
    };
}

#endif //GROWER_CONTROLLER_RPI_BUTTON_HPP
