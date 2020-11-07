#include <iostream>
#include <wiring/wiring_context.hpp>
#include <wiring/gpio.hpp>
#include <peripheral/case_fan.hpp>
#include <peripheral/power_supply.hpp>
#include <utils/log.hpp>
#include <http/http_server.hpp>
#include <net/websocket_server.hpp>
#include <ui/gl/mesh.hpp>
#include "context.hpp"
#include "video/video_manager.hpp"
#include "net/websocket_client.hpp"
#include "io/byte_buffer.hpp"
#include "ui/gl/graphics_device.hpp"
#include <tslib.h>
#include "ui/ui_manager.hpp"

namespace grower {
    int main() {
        utils::logger log{"main"};
        log->info("Touch versions: tslib: {}", tslib_version());

        auto ws = std::make_shared<net::websocket_server>(8085);
        ws->add_route("/camera", [](net::websocket_packet ignored) {}, [](const net::websocket_client_ptr &client) {
            const auto key = s_video_mgr->add_frame_callback([client](const std::vector<uint8_t> &data, std::size_t w, std::size_t h) {
                io::byte_buffer buff{};
                buff.append((uint32_t) w).append((uint32_t) h).append(data);
                client->send_message(buff.buffer());
            });

            client->tag(key);
        }, [](const net::websocket_client_ptr &client) {
            const auto tag = client->tag();
            if (!tag.has_value()) {
                return;
            }

            const auto key = std::any_cast<std::size_t>(client->tag());
            s_video_mgr->remove_frame_callback(key);
        });

        http::http_server web_server{8081, std::make_shared<grower::context>()};

        s_graphics_device->initialize();

        s_ui_mgr->initialize();

        while (!s_ui_mgr->should_quit()) {
            s_graphics_device->begin_frame();
            s_ui_mgr->render();
            s_graphics_device->end_frame();
        }

/*      wiring::wiring_context context{};
        auto app_context = std::make_shared<grower::context>();
        app_context->setup();

        log->info("System ready");

        http::http_server web_server{8081, app_context};*/

        return 0;
    }
}

int main() {
    return grower::main();
}
