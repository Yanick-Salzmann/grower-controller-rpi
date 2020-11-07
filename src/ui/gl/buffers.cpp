#include "buffers.hpp"

namespace grower::ui::gl {

    buffer::buffer(GLenum type) : _type{type} {
        glGenBuffers(1, &_buffer);
    }

    void buffer::bind() {
        glBindBuffer(_type, _buffer);
    }

    void buffer::data(const void *data, std::size_t size) {
        bind();
        glBufferData(_type, size, data, GL_STATIC_DRAW);
    }
}