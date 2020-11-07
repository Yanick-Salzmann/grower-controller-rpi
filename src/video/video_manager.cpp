#include "video_manager.hpp"

namespace grower::video {
    LOGGER_IMPL(video_manager);

    video_manager::video_manager() {
        _server_thread = std::thread{[this]() {
            _server.listen("127.0.0.1", 8082);
        }};

        _camera_stream = std::make_shared<raw_camera_stream>();
        try {
            _camera_stream->initialize_camera(camera_configuration(600, 480, 22, 60));
        } catch (std::exception &e) {
            log->warn("Camera unavailable, images wont be available");
            return;
        }

        _camera_stream->add_data_callback([this](const std::vector<uint8_t> &data, std::size_t size) {
            handle_camera_frame(std::vector<uint8_t>{data.begin(), data.begin() + size});
        });

        _camera_stream->start_capture();
    }

    video_manager::~video_manager() {
        _server.stop();
        _server_thread.join();
    }

    std::size_t video_manager::add_frame_callback(const frame_callback_t &callback) {
        const auto key = ++_callback_counter;
        std::lock_guard<std::mutex> l{_callback_lock};
        _frame_callbacks.emplace(key, callback);
        return key;
    }

    void video_manager::handle_camera_frame(const std::vector<uint8_t> &data) {
        const auto width = _camera_stream->actual_width();
        const auto height = _camera_stream->actual_height();

        std::lock_guard<std::mutex> l{_callback_lock};
        for (const auto &callback : _frame_callbacks) {
            callback.second(data, width, height);
        }
    }

    void video_manager::remove_frame_callback(std::size_t key) {
        std::lock_guard<std::mutex> l{_callback_lock};
        _frame_callbacks.erase(key);
    }
}