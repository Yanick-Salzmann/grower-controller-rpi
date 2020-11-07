#ifndef GROWER_CONTROLLER_RPI_GRAPHICS_DEVICE_HPP
#define GROWER_CONTROLLER_RPI_GRAPHICS_DEVICE_HPP

#include <cstdint>
#include <memory>

#include <GLES3/gl3.h>
#include "glad_egl.h"
#undef countof

#include <xf86drm.h>
#include <xf86drmMode.h>
#include <gbm.h>

#include "utils/log.hpp"
#include "utils/singleton.hpp"

#include <chrono>

namespace grower::ui::gl {
    class graphics_device_drm : public utils::singleton<graphics_device_drm> {
        LOGGER;

        int32_t _device = -1;
        uint32_t _connector_id;
        EGLDisplay _display{};
        EGLContext _context{};
        EGLSurface _surface{};

        uint32_t _width;
        uint32_t _height;

        gbm_device *_gbm_device{};
        gbm_surface *_gbm_surface{};
        gbm_bo *_previous_bo{};
        uint32_t _previous_fb{};
        drmModeCrtcPtr _crtc{};
        drmModeModeInfo _mode{};

        void gbm_reset();

        void context_setup();

        void load_drm_resources();

        void load_gbm_resources();

        void initialize_egl();

        EGLConfig find_matching_egl_config();

    public:
        ~graphics_device_drm() {
            gbm_reset();
        }

        void initialize();

        void begin_frame();

        void end_frame();
    };
}


#define s_graphics_device (grower::ui::gl::graphics_device_drm::instance())

#endif //GROWER_CONTROLLER_RPI_GRAPHICS_DEVICE_HPP
