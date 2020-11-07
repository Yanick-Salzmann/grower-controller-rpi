#include "color_quad.hpp"

namespace grower::ui::elements {
    gl::mesh_ptr color_quad::_mesh{};

    void color_quad::update_ortho_matrix(const glm::mat4 &matrix) {
        _mesh->program()->set_matrix("ortho", matrix);
    }

    void color_quad::initialize_mesh() {
        _mesh = std::make_shared<gl::mesh>();

        const auto prog = std::make_shared<gl::program>();
        prog->compile_vertex_shader(R"(
attribute vec2 position0;
attribute vec4 color0;

uniform mat4 ortho;

varying vec4 color;

void main() {
    color = color0;
    gl_Position = ortho * vec4(position0, 0.0, 1.0);
}
)");
        prog->compile_fragment_shader(R"(
varying vec4 color;

void main() {
    gl_FragColor = color;
}
)");

        prog->link();

        _mesh->index_buffer()->data<uint16_t>({0, 1, 2, 0, 2, 3});
        _mesh->index_count(6)
                .program(prog)
                .add_element("position", 0, 2)
                .add_element("color", 0, 4, gl::data_type::byte, true)
                .finalize();
    }

    void color_quad::update_vertex_buffer() {
#pragma pack(push, 1)
        struct vertex {
            glm::vec2 position;
            uint32_t color;
        };
#pragma pack(pop)

        vertex vertices[4]{
                {_position,                                      _color},
                {{_position.x + _size.x, _position.y},           _color},
                {_position + _size,                              _color},
                {{_position.x,           _position.y + _size.y}, _color}
        };

        if (!_vertex_buffer) {
            _vertex_buffer = std::make_shared<gl::vertex_buffer>();
        }

        _vertex_buffer->data(vertices);
    }

    void color_quad::render() {
        _mesh->vertex_buffer(_vertex_buffer);
        _mesh->begin_draw();
        _mesh->draw();
    }
}