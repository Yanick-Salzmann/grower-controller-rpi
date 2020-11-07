#ifndef GROWER_CONTROLLER_RPI_MESH_HPP
#define GROWER_CONTROLLER_RPI_MESH_HPP

#include <GLES3/gl3.h>
#include "glad_egl.h"
#include <string>
#include <utility>
#include <vector>
#include "program.hpp"
#include "buffers.hpp"

namespace grower::ui::gl {
    enum class data_type {
        floating = GL_FLOAT,
        byte = GL_UNSIGNED_BYTE
    };

    class vertex_element {
        LOGGER;

        std::string _semantic;
        uint32_t _index;
        uint32_t _components;
        data_type _data_type;
        bool _do_normalize = false;

        uint32_t _offset = 0;
        int32_t _attribute_index = -1;

    public:
        vertex_element(std::string semantic, uint32_t index, uint32_t num_components,
                       data_type data_type = data_type::floating, bool do_normalize = false);

        uint32_t initialize(const program_ptr &program, uint32_t offset);

        void activate(uint32_t stride) const;
    };

    enum class blend_mode {
        alpha,
        color,
        none
    };

    class blend_state {
        GLenum _src_blend = GL_ZERO;
        GLenum _dst_blend = GL_ONE;

        bool _is_enabled = false;

    public:
        blend_state mode(const blend_mode &mode) {
            switch (mode) {
                case blend_mode::alpha:
                    _is_enabled = true;
                    return this->mode(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                case blend_mode::color:
                    _is_enabled = true;
                    return this->mode(GL_ONE, GL_ONE);
                case blend_mode::none:
                    _is_enabled = false;
                    return this->mode(GL_ZERO, GL_ONE);
                default:
                    throw std::runtime_error{"Invalid blend mode specified"};
            }
        }

        blend_state &mode(GLenum src_blend, GLenum dst_blend) {
            _src_blend = src_blend;
            _dst_blend = dst_blend;

            return *this;
        }

        [[nodiscard]] GLenum src_blend() const {
            return _src_blend;
        }

        [[nodiscard]] GLenum dst_blend() const {
            return _dst_blend;
        }

        [[nodiscard]] bool is_enabled() const {
            return _is_enabled && (_src_blend != GL_ZERO || _dst_blend != GL_ONE);
        };

        blend_state &disable() {
            _is_enabled = false;
            return *this;
        }

        blend_state &enable() {
            _is_enabled = true;
            return *this;
        }
    };

    class vertex_buffer;

    class index_buffer;

    typedef std::shared_ptr<vertex_buffer> vertex_buffer_ptr;
    typedef std::shared_ptr<index_buffer> index_buffer_ptr;

    class mesh {
        LOGGER;

        std::vector<vertex_element> _elements{};

        GLuint _vertex_attrib_obj = 0;

        vertex_buffer_ptr _vertex_buffer;
        index_buffer_ptr _index_buffer;
        program_ptr _program;

        uint32_t _num_indices = 0;
        uint32_t _stride = 0;

        blend_state _blend_state{};

    public:
        mesh();

        void finalize();

        [[nodiscard]] uint32_t index_count() const {
            return _num_indices;
        }

        mesh &index_count(uint32_t num_indices) {
            _num_indices = num_indices;
            return *this;
        }

        [[nodiscard]] vertex_buffer_ptr vertex_buffer() const {
            return _vertex_buffer;
        }

        mesh &vertex_buffer(vertex_buffer_ptr new_vertex_buffer) {
            _vertex_buffer = std::move(new_vertex_buffer);
            return *this;
        }

        [[nodiscard]] index_buffer_ptr index_buffer() const {
            return _index_buffer;
        }

        mesh &index_buffer(index_buffer_ptr new_index_buffer) {
            _index_buffer = std::move(new_index_buffer);
            return *this;
        }

        [[nodiscard]] program_ptr program() const {
            return _program;
        }

        mesh &program(program_ptr program) {
            _program = std::move(program);
            return *this;
        }

        mesh &add_element(vertex_element &&element) {
            _elements.emplace_back(element);
            return *this;
        }

        template<typename... Args>
        mesh &add_element(Args &&... args) {
            _elements.emplace_back(std::forward<Args>(args)...);
            return *this;
        }

        gl::blend_state &blend() {
            return _blend_state;
        }

        void begin_draw();

        void draw();
    };

    typedef std::shared_ptr<mesh> mesh_ptr;
}

#endif //GROWER_CONTROLLER_RPI_MESH_HPP
