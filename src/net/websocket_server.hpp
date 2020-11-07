#ifndef GROWER_CONTROLLER_RPI_WEBSOCKET_SERVER_HPP
#define GROWER_CONTROLLER_RPI_WEBSOCKET_SERVER_HPP

#include <string>
#include <vector>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include "net_socket.hpp"
#include "server_socket.hpp"

namespace grower::net {
    class websocket_client;

    struct websocket_packet {
        std::string route{};
        std::vector<uint8_t> payload{};
    };

    class websocket_server : public std::enable_shared_from_this<websocket_server> {
        typedef std::function<void(const websocket_packet &)> packet_callback_t;
        typedef std::function<void(std::shared_ptr<websocket_client>)> connect_callback_t;
        typedef std::function<void(std::shared_ptr<websocket_client>)> disconnect_callback_t;

        std::map<std::string, packet_callback_t> _packet_callbacks{};
        std::map<std::string, connect_callback_t> _connect_callbacks{};
        std::map<std::string, disconnect_callback_t> _disconnect_callbacks{};

        std::mutex _client_lock{};
        std::set<std::shared_ptr<websocket_client>> _clients{};

        server_socket _server;

        void client_connected(const net_socket_ptr& client);
    public:
        explicit websocket_server(uint16_t port = 8085);

        void add_route(const std::string &name, packet_callback_t on_packet, connect_callback_t on_connect = {}, disconnect_callback_t on_disconnect = {});

        void remove_client(const std::shared_ptr<websocket_client>& client);

        void handle_message(const std::string& path, const std::vector<uint8_t>& data);
        void handle_session(const std::string& path, const std::shared_ptr<websocket_client>& client);
    };

    typedef std::shared_ptr<websocket_server> websocket_server_ptr;
    typedef std::weak_ptr<websocket_server> websocket_server_weak_ptr;
}

#endif //GROWER_CONTROLLER_RPI_WEBSOCKET_SERVER_HPP
