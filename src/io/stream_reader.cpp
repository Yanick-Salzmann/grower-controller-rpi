#include "stream_reader.hpp"

namespace grower::io {

    stream_reader::stream_reader(const std::vector<uint8_t> &data) {
        std::string str_data{data.begin(), data.end()};
        _stream.str(str_data);
    }

    std::string stream_reader::read_line() {
        bool has_read;
        auto ret = read_line(has_read);
        if (!has_read) {
            throw std::runtime_error{"Error reading line from stream"};
        }

        return ret;
    }

    std::size_t stream_reader::consumed() {
        const auto cur = _stream.tellg();
        _stream.seekg(0, std::ios::beg);
        const auto start = _stream.tellg();
        _stream.seekg(cur, std::ios::cur);
        return cur - start;
    }

    std::string stream_reader::read_line(bool &has_read) {
        std::string ret{};
        if (std::getline(_stream, ret)) {
            has_read = true;
            return ret;
        }

        has_read = false;
        return {};
    }
}