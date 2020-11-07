#ifndef GROWER_CONTROLLER_RPI_FONT_AWESOME_IMAGE_HPP
#define GROWER_CONTROLLER_RPI_FONT_AWESOME_IMAGE_HPP

#include "ui/text/text_sprite.hpp"
#include "ui/text/font-awesome-icons.hpp"
#include "element.hpp"

namespace grower::ui::elements {
    class font_awesome_image : public element {
        glm::vec2 _position{};

        text::text_sprite _image;
    public:
        font_awesome_image();

        explicit font_awesome_image(text::font_awesome icon);

        font_awesome_image(text::font_awesome icon, const glm::vec2 &position) : font_awesome_image(icon) {
            this->position(position);
        }

        font_awesome_image(text::font_awesome icon, float x, float y) : font_awesome_image(icon, {x, y}) {

        }

        font_awesome_image &color(uint32_t color) {
            _image.color(color);
            return *this;
        }

        font_awesome_image &position(const glm::vec2 &position) {
            _image.position(position);
            return *this;
        }

        font_awesome_image &position(float x, float y) {
            return position(glm::vec2{x, y});
        }

        font_awesome_image &size(float size) {
            _image.font_size(size);
            return *this;
        }

        font_awesome_image &icon(text::font_awesome font_icon);

        glm::vec2 position() override {
            return _image.position();
        }

        glm::vec2 size() override {
            return _image.dimension();
        }

        void render() override;
    };
}

#endif //GROWER_CONTROLLER_RPI_FONT_AWESOME_IMAGE_HPP
