#include "websocket_client.hpp"
#include <sys/socket.h>
#include <utils/error.hpp>
#include <utils/string.hpp>
#include <io/stream_reader.hpp>
#include <regex>
#include "crypto/sha1.hpp"
#include "io/byte_buffer.hpp"
#include "websocket_server.hpp"

namespace grower::net {
    LOGGER_IMPL(websocket_client);

    websocket_client::websocket_client(const net_socket_ptr &sock, websocket_server_weak_ptr server) :
            _ws_server{std::move(server)} {
        _socket = sock;
        _is_reading = true;
        _read_thread = std::thread{[this]() { read_callback(); }};
    }

    websocket_client::~websocket_client() {
        _is_reading = false;
        if(std::this_thread::get_id() == _read_thread.get_id()) {
            _read_thread.detach();
            return;
        }

        if (_read_thread.joinable()) {
            _read_thread.join();
        }
    };

    void websocket_client::read_callback() {
        uint8_t chunk[16384]{};
        auto none_read = false;

        while (_is_reading) {
            const auto read = _socket->read_some(chunk);
            if (read < 0) {
                log->warn("Error reading from client websocket: {}", utils::error_to_string());
                break;
            }

            if (!read) {
                if (!none_read) {
                    std::this_thread::sleep_for(std::chrono::milliseconds{100});
                    none_read = true;
                    continue;
                } else {
                    log->warn("No data read from socket in 2 iterations, disconnecting");
                    break;
                }
            }

            none_read = false;
            std::vector<uint8_t> data{chunk, chunk + read};
            data_read(data);
        }
    }

    void websocket_client::data_read(const std::vector<uint8_t> &data) {
        _read_buffer.insert(_read_buffer.end(), data.begin(), data.end());
        maybe_process_request();
    }

    void websocket_client::maybe_process_request() {
        if (!_is_upgraded) {
            maybe_read_upgrade_request();
        } else {
            maybe_read_websocket_data();
        }
    }

    void websocket_client::maybe_read_upgrade_request() {
        io::stream_reader reader{_read_buffer};
        if (!try_read_status_line(reader)) {
            return;
        }

        if (!try_read_headers(reader)) {
            return;
        }

        determine_content_length();
        _read_buffer.erase(_read_buffer.begin(), _read_buffer.begin() + reader.consumed() + _content_length);

        auto itr = _header_map.find("upgrade");
        if (itr == _header_map.end() || utils::to_lower(itr->second) != "websocket") {
            log->warn("No/invalid upgrade header found in websocket request");
            shutdown();
            return;
        }

        itr = _header_map.find("sec-websocket-key");
        if (itr == _header_map.end()) {
            log->warn("No sec-websocket-key found in websocket request");
            shutdown();
            return;
        }

        _is_upgraded = true;
        send_upgrade_response(itr->second);
        _ws_server.lock()->handle_session(_path, shared_from_this());

        maybe_read_websocket_data();
    }

    void websocket_client::maybe_read_websocket_data() {
        if (_read_buffer.size() < 2) {
            return;
        }

        const auto b0 = _read_buffer[0];
        const auto b1 = _read_buffer[1];

        const auto fin = (b0 & 0x80u) != 0u;
        const auto opcode = (b0 & 0x0Fu);

        const auto masked = (b1 & 0x80u) != 0u;
        uint64_t length = b1 & 0x7Fu;

        auto consumed = 2;

        if (length == 126) {
            if (_read_buffer.size() < 4) {
                return;
            }

            length = (((uint16_t) _read_buffer[3u]) << 8u) | (uint16_t) _read_buffer[4];
            consumed += 2;
        } else if (length == 127) {
            if (_read_buffer.size() < 10) {
                return;
            }

            length = 0;
            for (auto i = 0u; i < 8u; ++i) {
                length |= ((uint64_t) _read_buffer[2 + i]) << (56u - i * 8u);
            }

            consumed += 8;
        }

        uint32_t masking_key = 0;
        if (masked) {
            if (_read_buffer.size() < consumed + 4) {
                return;
            }

            masking_key = (((uint32_t) _read_buffer[consumed + 0]) << 0u) |
                          (((uint32_t) _read_buffer[consumed + 1]) << 8u) |
                          (((uint32_t) _read_buffer[consumed + 2]) << 16u) |
                          (((uint32_t) _read_buffer[consumed + 3]) << 24u);

            consumed += 4;
        }

        if (_read_buffer.size() < consumed + length) {
            return;
        }

        if (masking_key) {
            unmask(_read_buffer.begin() + consumed, _read_buffer.begin() + consumed + length, masking_key);
        }

        _current_message.insert(_current_message.end(), _read_buffer.begin() + consumed, _read_buffer.begin() + consumed + length);
        _read_buffer.erase(_read_buffer.begin(), _read_buffer.begin() + consumed + length);

        if (fin) {
            _ws_server.lock()->handle_message(_path, _current_message);
            _current_message.clear();
            if (opcode == 8) {
                log->info("Received close message on websocket");
                shutdown();
            }
        } else {
            if (opcode != 0) {
                log->warn("Expected continuation opcode (0) but got {}", opcode);
            }
        }
    }

    bool websocket_client::try_read_status_line(io::stream_reader &reader) {
        std::string first_line = reader.read_line();
        if (first_line.empty()) {
            return false;
        }

        if (first_line[first_line.size() - 1] != '\r') {
            return false;
        }

        static std::regex header_line_regex{"([^ ]+) ([^ ]+) ([^\r\n]+)\r"};
        std::smatch match{};
        if (!std::regex_match(first_line, match, header_line_regex)) {
            log->warn("Client sent invalid content. Line '{}' does not match regex for status line", first_line);
            return false;
        }

        const auto path = match[2].str();
        _path = path;
        return true;
    }

    bool websocket_client::try_read_headers(io::stream_reader &reader) {
        bool line_read = false;
        auto line = reader.read_line(line_read);
        _header_map.clear();

        while (line_read) {
            if (line.empty()) {
                return false;
            }

            if (line[0] == '\r') {
                return true;
            }

            if (line[line.size() - 1] != '\r') {
                return false;
            }

            auto colon_end = line.find(':');
            if (colon_end == std::string::npos) {
                _header_map.emplace(utils::trim(line), "");
            } else {
                _header_map.emplace(utils::to_lower(utils::trim(line.substr(0, colon_end))), utils::trim(line.substr(colon_end + 1)));
            }

            line = reader.read_line(line_read);
        }

        return false;
    }

    bool websocket_client::determine_content_length() {
        for (const auto &header : _header_map) {
            if (header.first == "content-length") {
                _content_length = std::stoull(header.second);
                return true;
            }
        }

        return false;
    }

    void websocket_client::shutdown() {
        _is_reading = false;
        _socket->shutdown();
        _ws_server.lock()->remove_client(shared_from_this());
    }

    void websocket_client::send_upgrade_response(const std::string &key) {
        std::stringstream resp{};
        resp << "HTTP/1.1 101 Switching Protocols\r\n"
             << "Upgrade: websocket\r\n"
             << "Connection: Upgrade\r\n"
             << "Sec-WebSocket-Accept: " << crypto::sha1::hash_base64(key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11") << "\r\n"
             << "\r\n";

        _socket->write(resp.str());
    }

    void websocket_client::unmask(std::vector<uint8_t>::iterator start, std::vector<uint8_t>::iterator end, uint32_t masking_key) {
        std::size_t offset = 0;
        for (auto cur = start; cur != end; ++cur) {
            const auto index = (offset++) % 4u;
            const auto xor_val = (masking_key & (((uint32_t) 0xFFu) << (index * 8))) >> (index * 8);
            *cur ^= xor_val;
        }
    }

    void websocket_client::send_message(const std::string &message) {
        return send_message(std::vector<uint8_t>{message.begin(), message.end()});
    }

    void websocket_client::send_message(const std::vector<uint8_t> &message) {
        if (!_is_reading) {
            return;
        }

        io::byte_buffer buffer{};
        uint8_t b0 = 0x82;
        buffer.append(b0);
        if (message.size() < 126) {
            buffer.append((uint8_t) message.size());
        } else if (message.size() <= std::numeric_limits<uint16_t>::max()) {
            buffer.append((uint8_t) 126)
                    .append<uint16_t, io::big_endian_trait>(message.size());
        } else {
            buffer.append((uint8_t) 127)
                    .append<uint64_t, io::big_endian_trait>(message.size());
        }

        buffer.append(message);

        try {
            _socket->write(buffer.buffer());
        } catch (std::runtime_error &) {
            log->debug("Error writing to socket, remote end closed");
            shutdown();
        }
    }

}