#ifndef GROWER_CONTROLLER_RPI_HTTP_SERVER_HPP
#define GROWER_CONTROLLER_RPI_HTTP_SERVER_HPP

#include <cstdint>
#include <httplib.h>
#include "context.hpp"
#include "utils/log.hpp"

namespace grower::http {
    class http_server {
        LOGGER;

        httplib::Server _server{};
        context_ptr _context;

        std::thread _server_thread;

        void define_routes();

        static void client_error(httplib::Response& resp, const std::string& reason, const std::string& body);

        void get_fan_speed(const httplib::Request& req, httplib::Response& resp);
        void put_fan_speed(const httplib::Request& req, httplib::Response& resp);

        void get_psu_state(const httplib::Request& req, httplib::Response& resp);
        void put_psu_state(const httplib::Request& req, httplib::Response& resp);

        void stop_application(httplib::Response& resp);

    public:
        explicit http_server(uint16_t port, context_ptr context);
        ~http_server();
    };
}

#endif //GROWER_CONTROLLER_RPI_HTTP_SERVER_HPP
