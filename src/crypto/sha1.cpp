#include "sha1.hpp"
#include <openssl/sha.h>
#include "utils/basen.hpp"

namespace grower::crypto {

    std::string sha1::hash_base64(const std::string &value) {
        uint8_t buffer[SHA_DIGEST_LENGTH]{};
        SHA1((unsigned char*) value.c_str(), value.size(), buffer);
        //std::reverse(buffer, buffer + SHA_DIGEST_LENGTH);
        std::string encoded{};
        bn::encode_b64(buffer, buffer + SHA_DIGEST_LENGTH, std::back_inserter(encoded));
        while(encoded.size() % 4) {
            encoded += '=';
        }
        return encoded;
    }
}