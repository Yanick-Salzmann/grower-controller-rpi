#ifndef GROWER_CONTROLLER_RPI_TEXTURE_HPP
#define GROWER_CONTROLLER_RPI_TEXTURE_HPP

#include <vector>
#include <memory>
#include <GLES3/gl3.h>
#include "glad_egl.h"
#include "utils/log.hpp"

namespace grower::ui::gl {
    class texture {
        LOGGER;

        static GLuint _default_texture;

        GLuint _texture;

        void create_texture();
    public:
        texture();

        void bind();

        void load_from_memory_argb(std::size_t width, std::size_t height, const std::vector<uint8_t>& data) {
            load_from_memory_argb(width, height, data.data(), data.size());
        }

        void load_from_memory_argb(std::size_t width, std::size_t height, const void* data, std::size_t size);

        void load_from_memory_yuv420p(std::size_t width, std::size_t height, const void* data, std::size_t size);

        static void init_default_texture();
    };

    typedef std::shared_ptr<texture> texture_ptr;
}

#endif //GROWER_CONTROLLER_RPI_TEXTURE_HPP
