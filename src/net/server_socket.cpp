#include "server_socket.hpp"
#include <arpa/inet.h>
#include "utils/error.hpp"
#include "net_socket.hpp"

namespace grower::net {
    LOGGER_IMPL(server_socket);

    server_socket::server_socket(uint16_t port, std::function<void(const net_socket_ptr &)> client_callback) :
            _client_callback{std::move(client_callback)} {
        if (!init_server_ipv6(port)) {
            log->info("Could not create IPv6 socket, trying to create IPv4");
            if (!init_server_ipv4(port)) {
                log->error("Could not create IPv6 socket or IPv4 socket, out of options");
                throw std::runtime_error("Could not create server socket");
            }
        }

        load_local_name();
        log->info("Server socket listening on {}", _local_name);

        begin_acceptor();
    }

    server_socket::~server_socket() {
        shutdown();
    }

    bool server_socket::init_server_ipv6(uint16_t port) {
        _socket = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
        if (_socket < 0) {
            log->warn("Cannot generate IPv6 socket: {}", utils::error_to_string());
            return false;
        }

        uint32_t reuse_addr = 1;
        auto rc = setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof reuse_addr);
        if(rc < 0) {
            log->warn("Cannot set SO_REUSEADDR on socket: {}", utils::error_to_string());
        }

        sockaddr_in6 sin{};
        sin.sin6_port = htons(port);
        sin.sin6_addr = IN6ADDR_ANY_INIT;
        sin.sin6_family = AF_INET6;

        rc = bind(_socket, (const sockaddr *) &sin, sizeof sin);
        if (rc < 0) {
            log->warn("Error binding IPv6 socket to network interface: {}", utils::error_to_string());
            close(_socket);
            return false;
        }

        rc = listen(_socket, 10);
        if (rc < 0) {
            log->warn("Error putting IPv6 socket into listening mode: {}", utils::error_to_string());
            close(_socket);
            return false;
        }

        return true;
    }

    void server_socket::load_local_name() {
        sockaddr_in6 addr{};
        socklen_t len = sizeof(addr);

        if (getsockname(_socket, (sockaddr *) &addr, &len) < 0) {
            log->warn("Could not determine local address: {}", utils::error_to_string());
            _local_name = "unknown:-1";
            return;
        }

        char name_buffer[INET6_ADDRSTRLEN + 1]{};
        if (!inet_ntop(addr.sin6_family, addr.sin6_family == AF_INET6 ? (const void *) &addr.sin6_addr : (const void *) &((sockaddr_in *) &addr)->sin_addr, name_buffer, INET6_ADDRSTRLEN)) {
            log->warn("Could not convert local IP address: {}", utils::error_to_string());
            _local_name = "unknown:-1";
            return;
        }

        if (addr.sin6_family == AF_INET) {
            _local_name = fmt::format("{}:{}", std::string{name_buffer}, htons(((sockaddr_in *) &addr)->sin_port));
        } else if (addr.sin6_family == AF_INET6) {
            _local_name = fmt::format("{}:{}", std::string{name_buffer}, htons(((sockaddr_in6 *) &addr)->sin6_port));
        } else {
            log->warn("Socket family {} is unknown", addr.sin6_family);
            _local_name = fmt::format("{}:-1", std::string{name_buffer});
        }
    }

    void server_socket::begin_acceptor() {
        _is_running = true;

        _listener_thread = std::thread{[this]() {
            while (_is_running) {
                sockaddr_in6 client_addr{};
                socklen_t client_len = sizeof client_addr;

                const auto client = accept(_socket, (sockaddr*) &client_addr, &client_len);
                if (client < 0) {
                    if (!_is_running) {
                        break;
                    }

                    log->warn("Error accepting new client: {}", utils::error_to_string());
                    break;
                }
                const auto socket_client = std::make_shared<net_socket>(client, *(sockaddr*)&client_addr);
                log->info("Accepted new client: {}", socket_client->to_string());
                if (_client_callback) {
                    _client_callback(socket_client);
                }
            }
        }};
    }

    bool server_socket::init_server_ipv4(uint16_t port) {
        _socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (_socket < 0) {
            log->warn("Error creating IPv4 socket: {}", utils::error_to_string());
            return false;
        }

        uint32_t reuse_addr = 1;
        auto rc = setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &reuse_addr, sizeof reuse_addr);
        if(rc < 0) {
            log->warn("Cannot set SO_REUSEADDR on socket: {}", utils::error_to_string());
        }

        sockaddr_in sin{};
        sin.sin_addr.s_addr = INADDR_ANY;
        sin.sin_port = htons(port);
        sin.sin_family = AF_INET;

        rc = bind(_socket, (const sockaddr *) &sin, sizeof sin);
        if (rc < 0) {
            close(_socket);
            log->warn("Error binding IPv4 socket to interface: {}", utils::error_to_string());
            return false;
        }

        rc = listen(_socket, 10);
        if (rc < 0) {
            close(_socket);
            log->warn("Error putting IPv4 socket into listening mode: {}", utils::error_to_string());
            return false;
        }

        return true;
    }

    void server_socket::shutdown() {
        _is_running = false;
        close(_socket);

        if (_listener_thread.joinable()) {
            _listener_thread.join();
        }
    }
}