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
            log->debug("{} {} --> {} {} ({} bytes body)", method, url, status, reason, body_size);
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

        _server.Get("/psu", [this](const auto &req, auto &resp) { get_psu_state(req, resp); });
        _server.Put("/psu", [this](const auto &req, auto &resp) { put_psu_state(req, resp); });

        _server.Get("/stop", [this](const auto &, auto &resp) { stop_application(resp); });
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

    void http_server::get_psu_state(const httplib::Request &req, httplib::Response &resp) {
        nlohmann::json ret{};
        ret["enabled"] = false; //_context->psu()->state();
        resp.set_content(ret.dump(), "application/json;charset=UTF-8");
        resp.status = 200;
        resp.reason = "OK";
    }

    void http_server::put_psu_state(const httplib::Request &req, httplib::Response &resp) {
        nlohmann::json doc{};
        try {
            doc = nlohmann::json::parse(req.body);
        } catch (std::exception &e) {
            log->warn("Could not parse incoming request: {}", e.what());
            resp.status = 400;
            resp.reason = "Could not parse incoming JSON";
            return;
        }

        const auto itr = doc.find("enabled");
        if (itr == doc.end()) {
            log->warn("Received request for PSU without enable/disable flag");
            resp.status = 400;
            resp.reason = "No 'enabled' field in payload";
            return;
        }

        const auto enable = itr.value().get<bool>();
        if (enable) {
            _context->psu()->activate();
        } else {
            _context->psu()->deactivate();
        }

        resp.status = 200;
        resp.reason = "OK";
        resp.set_content(R"({"status": "OK"})", "text/plain");
    }
}