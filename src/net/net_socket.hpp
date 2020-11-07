#ifndef GROWER_CONTROLLER_RPI_NET_SOCKET_HPP
#define GROWER_CONTROLLER_RPI_NET_SOCKET_HPP

#include <sys/socket.h>
#include <string>
#include <utils/log.hpp>
#include <utils/error.hpp>

namespace grower::net {
    class net_socket {
        LOGGER;

        int _socket = -1;
        std::string _remote_address{};
        std::string _local_address{};

        void resolve_local_address();

        void resolve_remote_address(const sockaddr &remote_addr);

        static std::string addr_to_string(const sockaddr *addr);

    public:
        explicit net_socket(int sock, const sockaddr &remote_addr);

        void shutdown();

        [[nodiscard]] std::string to_string() const {
            return fmt::format("<socket={} address={}>", _socket, _remote_address);
        }

        net_socket &write(const void *data, std::size_t length);

        template<typename T>
        net_socket &write(const T &value) {
            return write(&value, sizeof value);
        }

        template<typename T>
        net_socket &write(const std::vector<T> &data) {
            return write(data.data(), data.size() * sizeof(T));
        }

        template<typename T, std::size_t N>
        net_socket &write(const T(&data)[N]) {
            return write(data, N * sizeof(T));
        }

        template<typename T>
        net_socket &operator<<(const T &value) {
            return write(&value, sizeof value);
        }

        template<typename T>
        net_socket &operator<<(const std::vector<T> &value) {
            return write(value);
        }

        template<typename T, std::size_t N>
        net_socket &operator<<(const T(&data)[N]) {
            return write(data);
        }

        template<std::size_t N>
        std::size_t read_some(uint8_t (&data)[N]) {
            return read_some(data, N);
        }

        std::ptrdiff_t read_some(uint8_t *buffer, std::size_t size);
    };

    template<>
    inline net_socket &net_socket::write(const std::string &value) {
        return write(value.c_str(), value.length());
    }

    typedef std::shared_ptr<net_socket> net_socket_ptr;
}

#endif //GROWER_CONTROLLER_RPI_NET_SOCKET_HPP
