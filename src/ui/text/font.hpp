#ifndef GROWER_CONTROLLER_RPI_FONT_HPP
#define GROWER_CONTROLLER_RPI_FONT_HPP

#include <utils/log.hpp>
#include <memory>
#include <fontconfig/fontconfig.h>

namespace grower::ui::text {
    class font {
        LOGGER;

    public:
        static void add_font(const std::string& file_name);

        static void load_default_fonts();
    };

    typedef std::shared_ptr<font> font_ptr;
}

#endif //GROWER_CONTROLLER_RPI_FONT_HPP
