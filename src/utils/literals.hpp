#ifndef GROWER_CONTROLLER_RPI_LITERALS_HPP
#define GROWER_CONTROLLER_RPI_LITERALS_HPP

#include <cstdint>

inline std::size_t operator "" _sz(unsigned long long value) {
    return static_cast<std::size_t>(value);
}

#endif //GROWER_CONTROLLER_RPI_LITERALS_HPP
