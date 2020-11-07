#include "font.hpp"

namespace grower::ui::text {
    LOGGER_IMPL(font);

    void font::add_font(const std::string &file_name) {
        if(!FcConfigAppFontAddFile(FcConfigGetCurrent(), (FcChar8 *) file_name.c_str())) {
            log->warn("Error adding font: {}", file_name);
        }
    }

    void font::load_default_fonts() {
        std::string fonts[]{
          "TitilliumWeb-Black.ttf",
          "TitilliumWeb-Bold.ttf",
          "TitilliumWeb-BoldItalic.ttf",
          "TitilliumWeb-ExtraLight.ttf",
          "TitilliumWeb-ExtraLightItalic.ttf",
          "TitilliumWeb-Italic.ttf",
          "TitilliumWeb-Light.ttf",
          "TitilliumWeb-LightItalic.ttf",
          "TitilliumWeb-Regular.ttf",
          "TitilliumWeb-SemiBold.ttf",
          "TitilliumWeb-SemiBoldItalic.ttf",
          "Font Awesome 5 Free-Solid-900.otf"
        };

        for(const auto& font : fonts) {
            log->info("Adding font: {}", font);
            add_font(fmt::format("resources/{}", font));
        }
    }
}