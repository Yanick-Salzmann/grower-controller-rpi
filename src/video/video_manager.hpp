#ifndef GROWER_CONTROLLER_RPI_VIDEO_MANAGER_HPP
#define GROWER_CONTROLLER_RPI_VIDEO_MANAGER_HPP

#include "httplib.h"
#include "raw_camera_stream.hpp"
#include "utils/log.hpp"
#include "utils/singleton.hpp"

namespace grower::video {
    class video_manager : public utils::singleton<video_manager> {
    public:
        typedef std::function<void(const std::vector<uint8_t>&, std::size_t, std::size_t)> frame_callback_t;
    private:

        LOGGER;

        std::thread _server_thread{};

        httplib::Server _server{};
        std::shared_ptr<raw_camera_stream> _camera_stream{};

        std::mutex _callback_lock{};
        std::atomic_size_t _callback_counter{};
        std::map<std::size_t, frame_callback_t> _frame_callbacks{};

        void handle_camera_frame(const std::vector<uint8_t>& data);

    public:
        video_manager();

        ~video_manager();

        std::size_t add_frame_callback(const frame_callback_t& callback);
        void remove_frame_callback(std::size_t key);
    };
}

#define s_video_mgr (grower::video::video_manager::instance())

#endif //GROWER_CONTROLLER_RPI_VIDEO_MANAGER_HPP
