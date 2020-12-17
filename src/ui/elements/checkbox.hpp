#ifndef GROWER_CONTROLLER_RPI_CHECKBOX_HPP
#define GROWER_CONTROLLER_RPI_CHECKBOX_HPP

#include <functional>
#include "ui/text/text_sprite.hpp"
#include "ui/elements/font_awesome_image.hpp"
#include "ui/elements/element.hpp"

namespace grower::ui::elements {
    class checkbox : public element {
    public:
        static constexpr float CHECKBOX_SIZE = 50.0f;
        static constexpr float CHECKBOX_FONT_SIZE = 35.0f;
        static constexpr float CHECKBOX_TEXT_OFFSET = 5.0f;

    private:
        bool _is_checked = false;
        bool _is_pressed = false;

        std::function<void(bool)> _callback{};

        text::text_sprite_ptr _caption;
        font_awesome_image_ptr _checked_image;
        font_awesome_image_ptr _unchecked_image;

        glm::vec2 _position{};
        glm::vec2 _size{};

        void update_positions();

    public:
        checkbox();

        void touch_down(const input_event &e) override;

        void touch_up(const input_event &e) override;

        void render() override;

        checkbox& callback(const std::function<void(bool)>& callback) {
            _callback = callback;
            return *this;
        }

        checkbox& checked(bool checked) {
            _is_checked = checked;
            return *this;
        }

        [[nodiscard]] bool checked() const {
            return _is_checked;
        }

        glm::vec2 position() override;
        checkbox& position(float x, float y) {
            return position({x, y});
        }

        checkbox& position(const glm::vec2& pos);

        glm::vec2 size() override;

        checkbox &text(const std::string &caption) {
            _caption->text(caption);
            update_positions();
            return *this;
        }
    };
}

#endif //GROWER_CONTROLLER_RPI_CHECKBOX_HPP
