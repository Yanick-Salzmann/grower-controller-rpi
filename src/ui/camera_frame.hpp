#ifndef GROWER_CONTROLLER_RPI_CAMERA_FRAME_HPP
#define GROWER_CONTROLLER_RPI_CAMERA_FRAME_HPP

#include <mutex>
#include <vector>
#include "gl/texture.hpp"
#include "gl/mesh.hpp"
#include "ui/elements/element.hpp"

namespace grower::ui {
    class camera_frame : public elements::element {
        static gl::mesh_ptr _mesh;
        static gl::program_ptr _program;

        std::mutex _data_lock;
        std::vector<uint8_t> _data{};
        std::size_t _width = 0;
        std::size_t _height = 0;
        bool _is_dirty = false;

        gl::texture_ptr _texture{};

        void on_camera_frame(std::size_t width, std::size_t height, const std::vector<uint8_t> &data);

    public:
        camera_frame();

        void render() override;

        glm::vec2 position() override;

        glm::vec2 size() override;

        static void initialize_mesh();

        static void update_ortho_matrix(const glm::mat4 &ortho);
    };
}

#endif //GROWER_CONTROLLER_RPI_CAMERA_FRAME_HPP
