#ifndef GROWER_CONTROLLER_RPI_ERROR_HPP
#define GROWER_CONTROLLER_RPI_ERROR_HPP

#include <string>
#include <errno.h>

namespace grower::utils {
    std::string error_to_string(int error_number = errno);
}

#endif //GROWER_CONTROLLER_RPI_ERROR_HPP
