#ifndef GROWER_CONTROLLER_RPI_BUFFERS_HPP
#define GROWER_CONTROLLER_RPI_BUFFERS_HPP

#include <vector>
#include <GLES3/gl3.h>
#include "glad_egl.h"

namespace grower::ui::gl {
    class buffer {
        GLuint _buffer;
        GLenum _type;

    protected:
        explicit buffer(GLenum type);

    public:
        void bind();

        void data(const void *data, std::size_t size);

        template<typename T>
        void data(const T &value) {
            data(&value, sizeof value);
        }

        template<typename T>
        void data(const std::vector<T> &values) {
            data(values.data(), values.size() * sizeof(T));
        }

        template<typename T, std::size_t N>
        void data(const T (&values)[N]) {
            data(values, N * sizeof(T));
        }
    };

    class vertex_buffer : public buffer {
    public:
        vertex_buffer() : buffer(GL_ARRAY_BUFFER) {

        }
    };

    enum class index_type {
        uint8 = GL_UNSIGNED_BYTE,
        uint16 = GL_UNSIGNED_SHORT,
        uint32 = GL_UNSIGNED_INT
    };

    class index_buffer : public buffer {
        index_type _type = index_type::uint16;

    public:
        index_buffer() : buffer(GL_ELEMENT_ARRAY_BUFFER) {

        }

        explicit index_buffer(index_type type) : _type{type}, buffer(GL_ELEMENT_ARRAY_BUFFER) {

        }

        [[nodiscard]] index_type type() const {
            return _type;
        }
    };
}

#endif //GROWER_CONTROLLER_RPI_BUFFERS_HPP
