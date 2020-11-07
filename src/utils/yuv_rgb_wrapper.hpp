#ifndef GROWER_CONTROLLER_RPI_YUV_RGB_WRAPPER_HPP
#define GROWER_CONTROLLER_RPI_YUV_RGB_WRAPPER_HPP

#include <cstdint>

namespace grower::utils {
    int yuv_to_rgb(const uint8_t *yuv, int width, int height, uint8_t *rgba);
}

#endif //GROWER_CONTROLLER_RPI_YUV_RGB_WRAPPER_HPP
