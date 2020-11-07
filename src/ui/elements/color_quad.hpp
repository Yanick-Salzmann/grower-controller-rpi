#ifndef GROWER_CONTROLLER_RPI_COLOR_QUAD_HPP
#define GROWER_CONTROLLER_RPI_COLOR_QUAD_HPP

#include "ui/gl/mesh.hpp"

namespace grower::ui::elements {
    class color_quad {
        static gl::mesh_ptr _mesh;

        gl::vertex_buffer_ptr _vertex_buffer;
        glm::vec2 _position{};
        glm::vec2 _size{};
        uint32_t _color{};

        void update_vertex_buffer();

    public:
        color_quad() = default;

        color_quad(float x, float y, float width, float height) : color_quad({x, y}, {width, height}) {

        }

        color_quad(float x, float y, float width, float height, uint32_t color) : color_quad({x, y}, {width, height}, color) {

        }

        color_quad(glm::vec2 position, glm::vec2 size) : color_quad(position, size, 0xFF000000) {

        }

        color_quad(glm::vec2 position, glm::vec2 size, uint32_t color) :
                _position{position},
                _size{size},
                _color{color} {
            update_vertex_buffer();
        }

        color_quad &position(glm::vec2 position) {
            _position = position;
            update_vertex_buffer();
            return *this;
        }

        color_quad &position(float x, float y) {
            _position.x = x;
            _position.y = y;
            update_vertex_buffer();
            return *this;
        }

        [[nodiscard]] glm::vec2 position() const {
            return _position;
        }

        color_quad &size(glm::vec2 size) {
            _size = size;
            update_vertex_buffer();
            return *this;
        }

        color_quad &size(float width, float height) {
            _size.x = width;
            _size.y = height;
            update_vertex_buffer();
            return *this;
        }

        [[nodiscard]] glm::vec2 size() const {
            return _size;
        }

        color_quad& color(uint32_t color) {
            _color = color;
            update_vertex_buffer();
            return *this;
        }

        color_quad& color(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 0xFF) {
            _color = (((uint32_t) a) << 24u) | (((uint32_t) r) << 16u) | (((uint32_t) g) << 8u) | (((uint32_t) b) << 0u);
            update_vertex_buffer();
            return *this;
        }

        void render();

        static void initialize_mesh();

        static void update_ortho_matrix(const glm::mat4 &matrix);
    };
}

#endif //GROWER_CONTROLLER_RPI_COLOR_QUAD_HPP
