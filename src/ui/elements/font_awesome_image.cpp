#include "font_awesome_image.hpp"
#include "ui/ui_manager.hpp"
#include "ui/text/font-awesome-icons.hpp"

namespace grower::ui::elements {

    font_awesome_image::font_awesome_image() : _image{ui_manager::font_awesome_name} {
        _image.font_size(80.0f);
    }

    font_awesome_image::font_awesome_image(text::font_awesome icon) : font_awesome_image() {
        _image.text(text::font_awesome_code(icon));
    }

    font_awesome_image &font_awesome_image::icon(text::font_awesome font_icon) {
        _image.text(text::font_awesome_code(font_icon));
        return *this;
    }

    void font_awesome_image::render() {
        _image.render();
    }
}