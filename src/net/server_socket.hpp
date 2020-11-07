#ifndef GROWER_CONTROLLER_RPI_SERVER_SOCKET_HPP
#define GROWER_CONTROLLER_RPI_SERVER_SOCKET_HPP

#include <sys/socket.h>
#include <cstdint>
#include <memory>
#include <functional>
#include "utils/log.hpp"

namespace grower::net {
    class net_socket;

    typedef std::shared_ptr<net_socket> net_socket_ptr;

    class server_socket {
        LOGGER;

        int _socket{};
        std::function<void(const net_socket_ptr &)> _client_callback{};

        std::thread _listener_thread{};
        volatile bool _is_running = false;

        std::string _local_name{};

        bool init_server_ipv6(uint16_t port);
        bool init_server_ipv4(uint16_t port);

        void begin_acceptor();

        void load_local_name();

    public:
        server_socket(uint16_t port, std::function<void(const net_socket_ptr &)> client_callback);
        ~server_socket();

        void shutdown();
    };
}

#endif //GROWER_CONTROLLER_RPI_SERVER_SOCKET_HPP
