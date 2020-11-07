#ifndef GROWER_CONTROLLER_RPI_WEBSOCKET_CLIENT_HPP
#define GROWER_CONTROLLER_RPI_WEBSOCKET_CLIENT_HPP

#include <thread>
#include <map>
#include <utils/log.hpp>
#include <io/stream_reader.hpp>
#include <net/net_socket.hpp>
#include <any>

namespace grower::net {
    class websocket_server;

    typedef std::shared_ptr<websocket_server> websocket_server_ptr;
    typedef std::weak_ptr<websocket_server> websocket_server_weak_ptr;

    class websocket_client : public std::enable_shared_from_this<websocket_client> {
        LOGGER;

        net_socket_ptr _socket{};
        bool _is_upgraded = false;

        bool _is_reading = false;
        std::thread _read_thread;

        std::string _path{};
        std::multimap<std::string, std::string> _header_map{};

        std::vector<uint8_t> _read_buffer{};
        std::size_t _content_length = 0;

        std::vector<uint8_t> _current_message{};

        std::any _tag{};

        websocket_server_weak_ptr _ws_server;

        void data_read(const std::vector<uint8_t> &data);

        void maybe_process_request();

        void maybe_read_upgrade_request();

        void send_upgrade_response(const std::string& key);

        void maybe_read_websocket_data();

        bool try_read_status_line(io::stream_reader &reader);

        bool try_read_headers(io::stream_reader &reader);

        bool determine_content_length();

        void read_callback();

        void shutdown();

        static void unmask(std::vector<uint8_t>::iterator start, std::vector<uint8_t>::iterator end, uint32_t masking_key);

    public:
        explicit websocket_client(const net_socket_ptr& sock, websocket_server_weak_ptr server);

        ~websocket_client();

        void send_message(const std::string& message);
        void send_message(const std::vector<uint8_t>& message);

        const std::string& path() const {
            return _path;
        }

        std::any tag() const {
            return _tag;
        }

        void tag(std::any tag) {
            _tag = std::move(tag);
        }
    };

    typedef std::shared_ptr<websocket_client> websocket_client_ptr;
}

#endif //GROWER_CONTROLLER_RPI_WEBSOCKET_CLIENT_HPP
