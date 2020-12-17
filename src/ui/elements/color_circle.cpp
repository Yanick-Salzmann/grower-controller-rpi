#include "color_circle.hpp"

namespace grower::ui::elements {
    gl::mesh_ptr color_circle::_mesh{};

    int32_t color_circle::_uniform_world = -1;
    int32_t color_circle::_uniform_color = -1;
    int32_t color_circle::_uniform_ortho = -1;
    int32_t color_circle::_uniform_center = -1;
    int32_t color_circle::_uniform_radius = -1;

    void color_circle::initialize_mesh() {
        _mesh = std::make_shared<gl::mesh>();

        const auto prog = std::make_shared<gl::program>();
        prog->compile_vertex_shader(R"(
attribute vec2 position0;

uniform mat4 ortho;
uniform mat4 world;

varying vec2 view_pos;

void main() {
    vec4 world_pos = world * vec4(position0, 0.0, 1.0);
    view_pos = world_pos.xy / world_pos.w;
    gl_Position = ortho * world_pos;
}
)");

        prog->compile_fragment_shader(R"(
varying vec2 view_pos;

uniform vec4 color;
uniform vec2 center;
uniform float radius;

void main() {
    float d = distance(center, view_pos);
    float factor = step(d, radius);
    float outer_factor = step(radius, d);
    float mixed = mix(1.0, 0.0, max(min((d - radius) / 2.0, 1.0), 0.0));
    outer_factor *= mixed;
    factor = max(factor, outer_factor);
    gl_FragColor = vec4(color.rgb, factor);
}
)");

        prog->link();

        _mesh->program(prog)
                .index_count(6)
                .add_element("position", 0, 2)
                .finalize();

        _mesh->blend().mode(gl::blend_mode::alpha);

        _mesh->index_buffer()->data<uint16_t>({0, 1, 2, 0, 2, 3});
        _mesh->vertex_buffer()->data<float>({
                                                    -1, -1,
                                                    1, -1,
                                                    1, 1,
                                                    -1, 1
                                            });

        _uniform_color = prog->uniform_location("color");
        _uniform_world = prog->uniform_location("world");
        _uniform_ortho = prog->uniform_location("ortho");
        _uniform_center = prog->uniform_location("center");
        _uniform_radius = prog->uniform_location("radius");
    }

    void color_circle::update_ortho_matrix(const glm::mat4 &matrix) {
        _mesh->program()->set_matrix(_uniform_ortho, matrix);
    }

    void color_circle::render() {
        _mesh->program()->set_matrix(_uniform_world, _world_matrix);
        _mesh->program()->set_vec4(_uniform_color, _color);
        _mesh->program()->set_float(_uniform_radius, _radius);
        _mesh->program()->set_vec2(_uniform_center, _position);

        _mesh->begin_draw();
        _mesh->draw();
    }

    void color_circle::update_size_position() {
        static const auto identity = glm::identity<glm::mat4>();
        _world_matrix = glm::translate(identity, glm::vec3{_position.x, _position.y, 0.0f});
        _world_matrix = glm::scale(_world_matrix, glm::vec3{_radius + 3.0f, _radius + 3.0f, 1.0f});
    }

    color_circle::color_circle() {
        update_size_position();
    }

    color_circle &color_circle::position(glm::vec2 position) {
        _position = position;
        update_size_position();
        return *this;
    }

    color_circle &color_circle::radius(float radius) {
        _radius = radius;
        update_size_position();
        return *this;
    }
}