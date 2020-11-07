#include "net_socket.hpp"
#include <arpa/inet.h>

namespace grower::net {
    LOGGER_IMPL(net_socket);

    net_socket::net_socket(int sock, const sockaddr &remote_addr) : _socket{sock} {
        resolve_local_address();
        resolve_remote_address(remote_addr);
    }

    void net_socket::resolve_local_address() {
        sockaddr_in6 addr{};
        socklen_t addr_length = sizeof(addr);
        if (getsockname(_socket, (sockaddr *) &addr, &addr_length) < 0) {
            log->warn("Could not determine remote address for socket {}: {}", _socket, utils::error_to_string());
            _local_address = "unknown";
            return;
        }

        _local_address = addr_to_string((const sockaddr*) &addr);
    }

    net_socket &net_socket::write(const void *data, std::size_t length) {
        auto num_written = 0;
        const auto ptr = (const uint8_t *) data;
        while (num_written < length) {
            const auto sent = send(_socket, ptr, length - num_written, 0);
            if (sent <= 0) {
                log->warn("Error writing to remote socket: {}", utils::error_to_string());
                throw std::runtime_error{"Error writing to remote socket"};
            }

            num_written += sent;
        }

        return *this;
    }

    void net_socket::resolve_remote_address(const sockaddr &addr) {
        _remote_address = addr_to_string(&addr);
    }

    std::string net_socket::addr_to_string(const sockaddr *addr) {
        char name_buffer[INET6_ADDRSTRLEN + 1]{};
        auto port = -1;
        auto inet_ntop_success = false;

        if (addr->sa_family == AF_INET) {
            inet_ntop_success = inet_ntop(addr->sa_family, &((sockaddr_in *) addr)->sin_addr, name_buffer, INET6_ADDRSTRLEN) != nullptr;
            port = static_cast<int32_t>(htons(((sockaddr_in *) &addr)->sin_port));
        } else if (addr->sa_family == AF_INET6) {
            inet_ntop_success = inet_ntop(addr->sa_family, &((sockaddr_in6 *) addr)->sin6_addr, name_buffer, INET6_ADDRSTRLEN) != nullptr;
            port = static_cast<int32_t>(htons(((sockaddr_in6 *) &addr)->sin6_port));
        }

        if (!inet_ntop_success) {
            log->warn("Could not convert IP address: {}", utils::error_to_string());
            return "unknown:-1";
        }


        name_buffer[sizeof name_buffer - 1] = '\0';

        return fmt::format("{}:{}", std::string{name_buffer}, port);
    }

    std::ptrdiff_t net_socket::read_some(uint8_t *buffer, std::size_t size) {
        return recv(_socket, buffer, size, 0);
    }

    void net_socket::shutdown() {
        close(_socket);
    }
}