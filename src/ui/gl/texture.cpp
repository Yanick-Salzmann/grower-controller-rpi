#include "texture.hpp"
#include <cassert>
#include "gl_call.hpp"

namespace grower::ui::gl {
    LOGGER_IMPL(texture);

    GLuint texture::_default_texture = 0;

    texture::texture() {
        _texture = _default_texture;
    }

    void texture::init_default_texture() {
        glGenTextures(1, &_default_texture);
        glBindTexture(GL_TEXTURE_2D, _default_texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, std::vector<uint32_t>{
            0xFFFF0000, 0xFF00FF00,
            0xFF0000FF, 0xFFFF7F3F
        }.data());

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void texture::create_texture() {
        glGenTextures(1, &_texture);
        bind();

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void texture::bind() {
        glBindTexture(GL_TEXTURE_2D, _texture);
    }

    void texture::load_from_memory_argb(std::size_t width, std::size_t height, const void *data, std::size_t size) {
        if(_texture == _default_texture) {
            create_texture();
        }

        assert(width * height * 4 == size);

        bind();
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    }

    void texture::load_from_memory_yuv420p(std::size_t width, std::size_t height, const void *data, std::size_t size) {
#define GL_YCBCR_MESA 0x8757
#define GL_UNSIGNED_SHORT_8_8_MESA 0x85BA
        if(_texture == _default_texture) {
            create_texture();
        }

        assert(width * height * 2 == size);

        bind();
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_YCBCR_MESA, width, height, 0, GL_YCBCR_MESA, GL_UNSIGNED_SHORT_8_8_MESA, data));
    }
}