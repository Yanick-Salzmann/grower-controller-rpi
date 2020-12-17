#ifndef GROWER_CONTROLLER_RPI_COLOR_CIRCLE_HPP
#define GROWER_CONTROLLER_RPI_COLOR_CIRCLE_HPP

#include "ui/gl/mesh.hpp"

namespace grower::ui::elements {
    class color_circle {
        static gl::mesh_ptr _mesh;
        static int32_t _uniform_world;
        static int32_t _uniform_color;
        static int32_t _uniform_ortho;
        static int32_t _uniform_center;
        static int32_t _uniform_radius;

        glm::mat4 _world_matrix{};
        glm::vec4 _color{1.0f, 1.0f, 1.0f, 1.0f};

        glm::vec2 _position{};
        float _radius = 1.0f;

        void update_size_position();

    public:
        color_circle();

        void render();

        color_circle &position(glm::vec2 position);

        color_circle &position(float x, float y) {
            return position({x, y});
        }

        color_circle &radius(float radius);

        color_circle &color(uint32_t color) {
            return this->color(glm::vec4{
                    float(color & 0xFFu) / 255.0f,
                    float((color >> 8u) & 0xFFu) / 255.0f,
                    float((color >> 16u) & 0xFFu) / 255.0f,
                    float((color >> 24u) & 0xFFu) / 255.0f
            });
        }

        color_circle &color(glm::vec4 color) {
            _color = color;
            return *this;
        }

        static void initialize_mesh();

        static void update_ortho_matrix(const glm::mat4 &matrix);
    };
}

#endif //GROWER_CONTROLLER_RPI_COLOR_CIRCLE_HPP
