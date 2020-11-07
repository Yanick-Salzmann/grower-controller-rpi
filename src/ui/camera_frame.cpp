#include "camera_frame.hpp"
#include "video/video_manager.hpp"
#include "utils/yuv_rgb_wrapper.hpp"

namespace grower::ui {
    gl::mesh_ptr camera_frame::_mesh;
    gl::program_ptr camera_frame::_program;

    camera_frame::camera_frame() {
        _texture = std::make_shared<gl::texture>();

        s_video_mgr->add_frame_callback([this](const std::vector<uint8_t> &data, std::size_t width, std::size_t height) {
            this->on_camera_frame(width, height, data);
        });
    }

    void camera_frame::initialize_mesh() {
        _program = std::make_shared<gl::program>();
        _program->compile_vertex_shader(R"(
attribute vec2 position0;
attribute vec2 texCoord0;

varying vec2 texCoord;

uniform mat4 ortho;

void main() {
    texCoord = texCoord0;
    gl_Position = ortho * vec4(position0, 0.0, 1.0);
}
)");
        _program->compile_fragment_shader(R"(
varying vec2 texCoord;

uniform sampler2D videoTexture;

void main() {
    gl_FragColor = texture2D(videoTexture, texCoord);
}
)");

        _program->link();
        _program->set_int("videoTexture", 0);

        _mesh = std::make_shared<gl::mesh>();
        _mesh->add_element("position", 0, 2)
                .add_element("texCoord", 0, 2)
                .index_count(6)
                .program(_program)
                .finalize();

        _mesh->vertex_buffer()->data<float>({
                                                    1024.0f - 304.0f, 600.0f - 240.0f, 0.0f, 0.0f,
                                                    1024.0f, 600.0f - 240.0f, 1.0f, 0.0f,
                                                    1024.0f, 600.0f, 1.0f, 1.0f,
                                                    1024.0f - 304.0f, 600.0f, 0.0f, 1.0f
                                            });

        _mesh->index_buffer()->data<uint16_t>({0, 1, 2, 0, 2, 3});
    }

    void camera_frame::update_ortho_matrix(const glm::mat4 &ortho) {
        _program->set_matrix("ortho", ortho);
    }

    void camera_frame::on_camera_frame(std::size_t width, std::size_t height, const std::vector<uint8_t> &data) {
        std::vector<uint8_t> actual_data(width * height * 4);
        utils::yuv_to_rgb(data.data(), (int) width, (int) height, actual_data.data());

        std::lock_guard<std::mutex> l{_data_lock};
        _width = width;
        _height = height;
        _data = actual_data;
        _is_dirty = true;
    }

    void camera_frame::render() {
        if (_is_dirty) {
            std::lock_guard<std::mutex> l{_data_lock};
            _texture->load_from_memory_argb(_width, _height, _data);
            _is_dirty = false;
        }

        glActiveTexture(GL_TEXTURE0);
        _texture->bind();

        _mesh->begin_draw();
        _mesh->draw();
    }

    glm::vec2 camera_frame::position() {
        return glm::vec2(1024.0f - 304.0f, 600.0f - 240.0f);
    }

    glm::vec2 camera_frame::size() {
        return glm::vec2(304.0f, 240.0f);
    }
}