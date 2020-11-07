#ifndef GROWER_CONTROLLER_RPI_RAW_CAMERA_STREAM_HPP
#define GROWER_CONTROLLER_RPI_RAW_CAMERA_STREAM_HPP

#include "interface/mmal/mmal.h"
#include "interface/mmal/util/mmal_util.h"

#include <mutex>
#include <memory>
#include "utils/log.hpp"

namespace grower::video {
    enum class VideoFormat {
        H264
    };

    class camera_configuration {
        uint32_t _width;
        uint32_t _height;

        uint32_t _preview_fps;
        uint32_t _fps;

    public:
        camera_configuration(uint32_t width, uint32_t height, uint32_t preview_fps, uint32_t fps) : _width(width), _height(height), _preview_fps(preview_fps), _fps(fps) {}

        [[nodiscard]] uint32_t width() const {
            return _width;
        }

        camera_configuration& width(uint32_t width) {
            _width = width;
            return *this;
        }

        [[nodiscard]] uint32_t height() const {
            return _height;
        }

        camera_configuration& height(uint32_t height) {
            _height = height;
            return *this;
        }

        [[nodiscard]] uint32_t preview_fps() const {
            return _preview_fps;
        }

        camera_configuration& preview_fps(uint32_t preview_fps) {
            _preview_fps = preview_fps;
            return *this;
        }

        [[nodiscard]] uint32_t fps() const {
            return _fps;
        }

        camera_configuration& fps(uint32_t fps) {
            _fps = fps;
            return *this;
        }
    };

    class raw_camera_stream {
        LOGGER;

        std::mutex _data_read_lock{};

        std::shared_ptr<MMAL_POOL_T> _video_pool{};

        std::shared_ptr<MMAL_COMPONENT_T> _camera{};
        MMAL_PORT_T* _video_port = nullptr;

        std::size_t _actual_width = 0;
        std::size_t _actual_height = 0;

        std::vector<uint8_t> _buffer_data{};

        std::mutex _callback_lock;
        std::vector<std::function<void(const std::vector<uint8_t>&, std::size_t)>> _data_callback;

        static uint32_t map_format(VideoFormat format);

        static void camera_control_callback(MMAL_PORT_T* port, MMAL_BUFFER_HEADER_T* buffer);
        static void video_data_callback(MMAL_PORT_T* port, MMAL_BUFFER_HEADER_T* buffer);

        void handle_camera_control(MMAL_PORT_T *port, MMAL_BUFFER_HEADER_T *buffer);
        void handle_video_data(MMAL_PORT_T* port, MMAL_BUFFER_HEADER_T* buffer, std::shared_ptr<MMAL_BUFFER_HEADER_T>& buffer_ptr);

    public:
        void add_data_callback(std::function<void(const std::vector<uint8_t>&, std::size_t)> callback) {
            _data_callback.emplace_back(std::move(callback));
        }

        void initialize_camera(const camera_configuration& configuration);

        bool start_capture();
        void stop_capture();

        [[nodiscard]] std::size_t actual_width() const {
            return _actual_width;
        }

        [[nodiscard]] std::size_t actual_height() const {
            return _actual_height;
        }

        static uint32_t calculate_width(uint32_t width);
        static uint32_t calculate_height(uint32_t height);
    };
}

#endif //GROWER_CONTROLLER_RPI_RAW_CAMERA_STREAM_HPP
