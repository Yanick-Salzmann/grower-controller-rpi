#include "http_server.hpp"
#include <nlohmann/json.hpp>
#include <video/video_manager.hpp>
#include "utils/string.hpp"

namespace grower::http {
    LOGGER_IMPL(http_server);

    http_server::http_server(uint16_t port, context_ptr context) : _context{std::move(context)} {
        define_routes();
        _server.set_logger([](const httplib::Request &req, const httplib::Response &res) {
            const auto method = req.method;
            const auto url = req.path;

            const auto status = res.status;
            const auto reason = res.reason;
            const auto body_size = res.body.size();
            //log->debug("{} {} --> {} {} ({} bytes body)", method, url, status, reason, body_size);
        });

        _server_thread = std::thread{
                [this, port]() {
                    _server.listen("0.0.0.0", port);
                }
        };
    }

    http_server::~http_server() {
        _server.stop();
        _server_thread.join();
    }

    void http_server::define_routes() {
        _server.set_mount_point("/", "./web");

        _server.Get("/fans", [this](const auto &req, auto &resp) { get_fan_speed(req, resp); });
        _server.Put("/fans", [this](const auto &req, auto &resp) { put_fan_speed(req, resp); });

        _server.Get("/stop", [this](const auto &, auto &resp) { stop_application(resp); });

        _server.Get("/current", [this](const auto &req, auto &resp) { update_current(req, resp); });
        _server.Get("/current/lamp", [this](const auto &req, auto &resp) { get_flower_current(req, resp); });
        _server.Get("/current/rpi", [this](const auto &req, auto &resp) { get_rpi_current(req, resp); });

        _server.Get("/state/flower", [this](const auto &req, auto &resp) { get_state_flowering(req, resp); });
        _server.Get("/state/germination", [this](const auto &req, auto &resp) { get_state_germ(req, resp); });
        _server.Get("/state/illumination", [this](const auto &req, auto &resp) { get_state_illumination(req, resp); });

        _server.Post("/state/flower", [this](const auto& req, auto& resp) { put_state_flowering(req, resp); });
        _server.Post("/state/germination", [this](const auto& req, auto& resp) { put_state_germination(req, resp); });
        _server.Post("/state/illumination", [this](const auto& req, auto& resp) { put_state_illumination(req, resp); });
    }

    void http_server::get_fan_speed(const httplib::Request &req, httplib::Response &resp) {
        return;

        auto fans = static_cast<std::size_t>(fan_type::all);
        if (req.has_param("fans")) {
            fans = (std::size_t) std::stoull(req.get_param_value("fan"));
        }

        nlohmann::json resp_doc{};
        if ((fans & (std::size_t) fan_type::cooler) != 0) {
            resp_doc["cooler"] = _context->cooler_fan()->rpm();
        }

        resp.set_content(resp_doc.dump(), "application/json");
        resp.status = 200;
        resp.reason = "OK";
    }

    void http_server::put_fan_speed(const httplib::Request &req, httplib::Response &resp) {
        return;

        nlohmann::json req_body{};
        try {
            req_body = nlohmann::json::parse(req.body);
        } catch (nlohmann::json::parse_error &e) {
            return client_error(resp, "Invalid JSON body", e.what());
        }

        if (req_body.find("speed") == req_body.end()) {
            log->warn("Received updated speed request for fans, but no 'speed' parameter is present, ignoring");
            return;
        }

        auto fans = static_cast<std::size_t>(fan_type::all);
        if (req_body.find("fans") != req_body.end()) {
            fans = static_cast<std::size_t>(req_body["fans"].get<std::size_t>());
        }

        std::size_t speed;
        try {
            speed = req_body["speed"].get<std::size_t>();
        } catch (std::exception &e) {
            return client_error(resp, "Invalid speed (not a number)", e.what());
        }

        if (fans & static_cast<std::size_t>(fan_type::cooler)) {
            _context->cooler_fan()->speed(speed);
        }
    }

    void http_server::stop_application(httplib::Response &resp) {
        resp.status = 200;
        resp.reason = "OK";
        _server.stop();
    }

    void http_server::client_error(httplib::Response &resp, const std::string &reason, const std::string &body) {
        resp.status = 400;
        resp.reason = reason;
        resp.body = body;
    }

    void http_server::update_current(const httplib::Request &req, httplib::Response &resp) {
        if (req.has_param("lamp")) {
            log->info("Lamp current: {}", req.params.find("lamp")->second);
        }

        resp.status = 200;
        resp.reason = "OK";
    }

    void http_server::get_flower_current(const httplib::Request &req, httplib::Response &resp) {
        const auto current = _context->lamp_current();
        resp.set_content(fmt::format("{}", current), "text/plain");
        resp.status = 200;
        resp.reason = "OK";
    }

    void http_server::get_rpi_current(const httplib::Request &req, httplib::Response &resp) {
        const auto current = _context->rpi_current();
        resp.set_content(fmt::format("{}", current), "text/plain");
        resp.status = 200;
        resp.reason = "OK";
    }

    void http_server::get_state_flowering(const httplib::Request &req, httplib::Response &resp) {
        resp.set_content(_context->flower_lamp()->state() ? "1" : "0", "text/plain");
        resp.status = 200;
        resp.reason = "OK";
    }

    void http_server::get_state_germ(const httplib::Request &req, httplib::Response &resp) {
        resp.set_content(_context->germ_lamp()->state() ? "1" : "0", "text/plain");
        resp.status = 200;
        resp.reason = "OK";
    }

    void http_server::get_state_illumination(const httplib::Request &res, httplib::Response &resp) {
        resp.set_content(_context->cam_light()->state() ? "1" : "0", "text/plain");
        resp.status = 200;
        resp.reason = "OK";
    }

    void http_server::put_state_flowering(const httplib::Request &req, httplib::Response &resp) {
        update_relay_state(_context->flower_lamp(), req, resp);
    }

    void http_server::put_state_germination(const httplib::Request &req, httplib::Response &resp) {
        update_relay_state(_context->germ_lamp(), req, resp);
    }

    void http_server::put_state_illumination(const httplib::Request &req, httplib::Response &resp) {
        update_relay_state(_context->cam_light(), req, resp);
    }

    void http_server::update_relay_state(peripheral::relay_control_ptr relay, const httplib::Request &req, httplib::Response& resp) {
        nlohmann::json req_body{};
        try {
            req_body = nlohmann::json::parse(req.body);
        } catch (nlohmann::json::parse_error &e) {
            return client_error(resp, "Invalid JSON body", e.what());
        }

        if(req_body.find("enabled") == req_body.end()) {
            return client_error(resp, "Missing parameter 'enabled' in request", "Missing parameter");
        }

        try {
            const auto enabled = req_body["enabled"].get<bool>();
            std::cout << "Putting relay to " << (enabled ? "enabled" : "disabled") << " status" << std::endl;
            relay->state(enabled);
        } catch (std::exception& ex) {
            return client_error(resp, "Error unwrapping request", ex.what());
        }
    }
}