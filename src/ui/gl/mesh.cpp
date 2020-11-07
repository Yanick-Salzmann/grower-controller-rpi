#include "mesh.hpp"
#include "gl_call.hpp"

namespace grower::ui::gl {
    LOGGER_IMPL(mesh);
    LOGGER_IMPL(vertex_element);

    vertex_element::vertex_element(std::string semantic, uint32_t index, uint32_t num_components, data_type data_type, bool do_normalize) :
            _semantic(std::move(semantic)),
            _index(index),
            _components(num_components),
            _data_type(data_type),
            _do_normalize(do_normalize) {

    }

    uint32_t vertex_element::initialize(const program_ptr &program, uint32_t offset) {
        const auto attribute_name = fmt::format("{}{}", _semantic, _index);

        _offset = offset;
        _attribute_index = program->attribute_location(attribute_name);
        assert(_attribute_index >= 0);

        switch (_data_type) {
            case data_type::byte: {
                return _components;
            }

            default: {
                return _components * 4;
            }
        }
    }

    void vertex_element::activate(uint32_t stride) const {
        assert(_attribute_index >= 0);

        GL_CALL(glEnableVertexAttribArray(static_cast<GLuint>(_attribute_index)));
        GL_CALL(glVertexAttribPointer(static_cast<GLuint>(_attribute_index), _components,
                                      static_cast<GLenum>(_data_type),
                                      _do_normalize ? GL_TRUE : GL_FALSE, stride,
                                      reinterpret_cast<const void *>(_offset)
        ));
    }

    mesh::mesh() {
        _vertex_buffer = std::make_shared<gl::vertex_buffer>();
        _index_buffer = std::make_shared<gl::index_buffer>();

        GL_CALL(glGenVertexArrays(1, &_vertex_attrib_obj));
    }

    void mesh::finalize() {
        assert(_program != nullptr);
        assert(!_elements.empty());

        _stride = 0;

        for (auto &elem : _elements) {
            _stride += elem.initialize(_program, _stride);
        }
    }

    void mesh::begin_draw() {
        assert(_program != nullptr);
        assert(_vertex_buffer != nullptr);
        assert(_index_buffer != nullptr);

        GL_CALL(glBindVertexArray(_vertex_attrib_obj));

        _program->use();

        _vertex_buffer->bind();
        _index_buffer->bind();

        for (const auto &elem : _elements) {
            elem.activate(_stride);
        }
    }

    void mesh::draw() {
        if(!_blend_state.is_enabled()) {
            glDisable(GL_BLEND);
        } else {
            glEnable(GL_BLEND);
            glBlendFunc(_blend_state.src_blend(), _blend_state.dst_blend());
        }

        // _texture_state.apply();

        GL_CALL(glDrawElements(GL_TRIANGLES, _num_indices, static_cast<GLenum>(_index_buffer->type()), nullptr));
    }
}