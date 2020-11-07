#ifndef GROWER_CONTROLLER_RPI_SHA1_HPP
#define GROWER_CONTROLLER_RPI_SHA1_HPP

#include <string>

namespace grower::crypto {
    class sha1 {
    public:
        static std::string hash_base64(const std::string& value);
    };
}

#endif //GROWER_CONTROLLER_RPI_SHA1_HPP
