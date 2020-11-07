#ifndef GROWER_CONTROLLER_RPI_STREAM_READER_HPP
#define GROWER_CONTROLLER_RPI_STREAM_READER_HPP

#include <sstream>
#include <vector>

namespace grower::io {
    class stream_reader {
        std::istringstream _stream{};

    public:
        explicit stream_reader(const std::vector<uint8_t>& data);

        std::string read_line();
        std::string read_line(bool& has_read);

        std::size_t consumed();
    };
}

#endif //GROWER_CONTROLLER_RPI_STREAM_READER_HPP
