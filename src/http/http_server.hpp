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

        void get_flower_current(const httplib::Request& req, httplib::Response& resp);
        void get_rpi_current(const httplib::Request& req, httplib::Response& resp);
        void get_total_current(const httplib::Request& req, httplib::Response& resp);

        void get_state_flowering(const httplib::Request& req, httplib::Response& resp);
        void get_state_germ(const httplib::Request& req, httplib::Response& resp);
        void get_state_illumination(const httplib::Request& res, httplib::Response& resp);

        void put_state_flowering(const httplib::Request& req, httplib::Response& resp);
        void put_state_germination(const httplib::Request& req, httplib::Response& resp);
        void put_state_illumination(const httplib::Request& req, httplib::Response& resp);

        void update_relay_state(peripheral::relay_control_ptr relay, const httplib::Request& req, httplib::Response& resp);

        void update_current(const httplib::Request& req, httplib::Response& resp);

        void stop_application(httplib::Response& resp);

    public:
        explicit http_server(uint16_t port, context_ptr context);
        ~http_server();
    };
}

#endif //GROWER_CONTROLLER_RPI_HTTP_SERVER_HPP
