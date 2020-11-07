#include "websocket_server.hpp"
#include "websocket_client.hpp"
#include "utils/string.hpp"

namespace grower::net {
    websocket_server::websocket_server(uint16_t port) :
            _server{port, [this](const net_socket_ptr &client) { client_connected(client); }} {

    }

    void websocket_server::add_route(const std::string &name, websocket_server::packet_callback_t on_packet, websocket_server::connect_callback_t on_connect, disconnect_callback_t on_disconnect) {
        if (on_packet) {
            _packet_callbacks.emplace(name, std::move(on_packet));
        }

        if (on_connect) {
            _connect_callbacks.emplace(name, std::move(on_connect));
        }

        if (on_disconnect) {
            _disconnect_callbacks.emplace(name, std::move(on_disconnect));
        }
    }

    void websocket_server::remove_client(const websocket_client_ptr &client) {
        {
            std::lock_guard<std::mutex> l{_client_lock};
            _clients.erase(client);
        }
        auto itr = _disconnect_callbacks.find(utils::to_lower(client->path()));
        if (itr == _disconnect_callbacks.end()) {
            return;
        }

        itr->second(client);
    }

    void websocket_server::client_connected(const net_socket_ptr &client) {
        const auto client_ptr = std::make_shared<websocket_client>(client, shared_from_this());
        std::lock_guard<std::mutex> l{_client_lock};
        _clients.emplace(client_ptr);
    }

    void websocket_server::handle_message(const std::string &path, const std::vector<uint8_t> &data) {
        auto itr = _packet_callbacks.find(utils::to_lower(path));
        if (itr == _packet_callbacks.end()) {
            return;
        }

        itr->second(websocket_packet{
                path,
                data
        });
    }

    void websocket_server::handle_session(const std::string &path, const websocket_client_ptr &client) {
        auto itr = _connect_callbacks.find(utils::to_lower(path));
        if (itr == _connect_callbacks.end()) {
            return;
        }

        itr->second(client);
    }
}
