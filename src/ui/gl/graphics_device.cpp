#undef countof

#include "graphics_device.hpp"
#include <utils/error.hpp>
#include "texture.hpp"
#include "ui/text/text_sprite.hpp"
#include "ui/camera_frame.hpp"
#include "ui/elements/color_quad.hpp"

namespace grower::ui::gl {
    LOGGER_IMPL(graphics_device_drm);

    static const char *eglGetErrorStr() {
        switch (eglGetError()) {
            case EGL_SUCCESS:
                return "The last function succeeded without error.";
            case EGL_NOT_INITIALIZED:
                return "EGL is not initialized, or could not be initialized, for the "
                       "specified EGL display connection.";
            case EGL_BAD_ACCESS:
                return "EGL cannot access a requested resource (for example a context "
                       "is bound in another thread).";
            case EGL_BAD_ALLOC:
                return "EGL failed to allocate resources for the requested operation.";
            case EGL_BAD_ATTRIBUTE:
                return "An unrecognized attribute or attribute value was passed in the "
                       "attribute list.";
            case EGL_BAD_CONTEXT:
                return "An EGLContext argument does not name a valid EGL rendering "
                       "context.";
            case EGL_BAD_CONFIG:
                return "An EGLConfig argument does not name a valid EGL frame buffer "
                       "configuration.";
            case EGL_BAD_CURRENT_SURFACE:
                return "The current surface of the calling thread is a window, pixel "
                       "buffer or pixmap that is no longer valid.";
            case EGL_BAD_DISPLAY:
                return "An EGLDisplay argument does not name a valid EGL display "
                       "connection.";
            case EGL_BAD_SURFACE:
                return "An EGLSurface argument does not name a valid surface (window, "
                       "pixel buffer or pixmap) configured for GL rendering.";
            case EGL_BAD_MATCH:
                return "Arguments are inconsistent (for example, a valid context "
                       "requires buffers not supplied by a valid surface).";
            case EGL_BAD_PARAMETER:
                return "One or more argument values are invalid.";
            case EGL_BAD_NATIVE_PIXMAP:
                return "A NativePixmapType argument does not refer to a valid native "
                       "pixmap.";
            case EGL_BAD_NATIVE_WINDOW:
                return "A NativeWindowType argument does not refer to a valid native "
                       "window.";
            case EGL_CONTEXT_LOST:
                return "A power management event has occurred. The application must "
                       "destroy all contexts and reinitialise OpenGL ES state and "
                       "objects to continue rendering.";
            default:
                break;
        }
        return "Unknown error!";
    }

    static const EGLint config_attribs[] = {
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_DEPTH_SIZE, 8,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
            EGL_NONE
    };

    static const EGLint context_attribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 3,
            EGL_NONE
    };

    void graphics_device_drm::initialize() {
        load_drm_resources();
        load_gbm_resources();
        initialize_egl();

        _width = _mode.hdisplay;
        _height = _mode.vdisplay;

        context_setup();
    }

    void graphics_device_drm::begin_frame() {
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void graphics_device_drm::end_frame() {
        auto sync = eglCreateSyncKHR(_display, EGL_SYNC_FENCE_KHR, nullptr);
        glFlush();
        eglClientWaitSyncKHR(_display, sync, 0, EGL_FOREVER_KHR);

        eglSwapBuffers(_display, _surface);

        auto bo = gbm_surface_lock_front_buffer(_gbm_surface);
        const auto handle = gbm_bo_get_handle(bo).u32;
        const auto pitch = gbm_bo_get_stride(bo);

        uint32_t fb;
        drmModeAddFB(_device, _width, _height, 24, 32, pitch, handle, &fb);
        drmModeSetCrtc(_device, _crtc->crtc_id, fb, 0, 0, &_connector_id, 1, &_mode);

        if (_previous_bo) {
            drmModeRmFB(_device, _previous_fb);
            gbm_surface_release_buffer(_gbm_surface, _previous_bo);
        }

        _previous_bo = bo;
        _previous_fb = fb;
    }

    void graphics_device_drm::gbm_reset() {
        drmModeSetCrtc(_device, _crtc->crtc_id, _crtc->buffer_id, _crtc->x, _crtc->y, &_connector_id, 1, &_crtc->mode);
        drmModeFreeCrtc(_crtc);

        if (_previous_bo) {
            drmModeRmFB(_device, _previous_fb);
            gbm_surface_release_buffer(_gbm_surface, _previous_bo);
        }

        gbm_surface_destroy(_gbm_surface);
        gbm_device_destroy(_gbm_device);
    }

    void graphics_device_drm::context_setup() {
        log->info("OpenGL Vendor: {}", glGetString(GL_VENDOR));
        log->info("OpenGL Renderer: {}", glGetString(GL_RENDERER));
        log->info("OpenGL Version: {}", glGetString(GL_VERSION));
        log->info("OpenGL GLSL Version: {}", glGetString(GL_SHADING_LANGUAGE_VERSION));

        GLint extensions = 0;
        glGetIntegerv(GL_NUM_EXTENSIONS, &extensions);
        log->debug("Extensions:");
        for(auto i = 0; i < extensions; ++i) {
            log->debug("> #{} {}", i + 1, glGetStringi(GL_EXTENSIONS, i));
        }

        glViewport(0, 0, _width, _height);
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        texture::init_default_texture();
        text::text_sprite::init_mesh();
        camera_frame::initialize_mesh();
        elements::color_quad::initialize_mesh();

        const auto ortho = glm::ortho(0.0f, (float) _width, (float) _height, 0.0f, 0.0f, 1.0f);
        text::text_sprite::update_ortho_matrix(ortho);
        camera_frame::update_ortho_matrix(ortho);
        elements::color_quad::update_ortho_matrix(ortho);
    }

    void graphics_device_drm::load_drm_resources() {
        _device = open("/dev/dri/card1", O_RDWR | O_CLOEXEC);
        if (_device < 0) {
            log->error("Error opening /dev/dri/card1: {}", utils::error_to_string());
            throw std::runtime_error{"Error opening device"};
        }

        const auto resources = drmModeGetResources(_device);
        if (!resources) {
            log->error("Error calling drmModeGetResources: {}", utils::error_to_string());
            throw std::runtime_error{"Error getting DRM resources"};
        }

        drmModeConnector *connector = nullptr;
        for (auto i = 0; i < resources->count_connectors; ++i) {
            const auto conn = drmModeGetConnector(_device, resources->connectors[i]);
            if (conn->connection == DRM_MODE_CONNECTED) {
                connector = conn;
                break;
            }

            drmModeFreeConnector(conn);
        }

        if (!connector) {
            drmModeFreeResources(resources);
            log->error("No drmModeConnector found that is connected");
            throw std::runtime_error{"No drm mode connector found"};
        }

        _connector_id = connector->connector_id;

        auto mode = _mode = connector->modes[0];
        log->info("Screen resolution: {}x{} ({})", mode.hdisplay, mode.vdisplay, mode.name);

        auto encoder = connector->encoder_id ? drmModeGetEncoder(_device, connector->encoder_id) : nullptr;
        if (!encoder) {
            drmModeFreeResources(resources);
            drmModeFreeConnector(connector);
            log->error("Error getting DRM encoder");
            throw std::runtime_error{"No drm encoder found"};
        }

        _crtc = drmModeGetCrtc(_device, encoder->crtc_id);
        drmModeFreeEncoder(encoder);
        drmModeFreeConnector(connector);
        drmModeFreeResources(resources);
    }

    void graphics_device_drm::load_gbm_resources() {
        _gbm_device = gbm_create_device(_device);
        _gbm_surface = gbm_surface_create(_gbm_device, _mode.hdisplay, _mode.vdisplay, GBM_FORMAT_XRGB8888, GBM_BO_USE_SCANOUT | GBM_BO_USE_RENDERING);
        _display = eglGetDisplay(_gbm_device);
    }

    void graphics_device_drm::initialize_egl() {
        int32_t major, minor;
        if (eglInitialize(_display, &major, &minor) == EGL_FALSE) {
            eglTerminate(_display);
            gbm_reset();

            log->error("Error initializing EGL: {}", eglGetErrorStr());
            throw std::runtime_error{"Error initializing EGL"};
        }

        log->info("EGL version: {}.{}", major, minor);

        eglBindAPI(EGL_OPENGL_API);

        auto matching_config = find_matching_egl_config();

        _context = eglCreateContext(_display, matching_config, EGL_NO_CONTEXT, context_attribs);
        if (_context == EGL_NO_CONTEXT) {
            log->error("Error creating EGL context: {}", eglGetErrorStr());
            eglTerminate(_display);
            gbm_reset();
            throw std::runtime_error{"Error creating EGL context"};
        }

        _surface = eglCreateWindowSurface(_display, matching_config, _gbm_surface, nullptr);
        if (_surface == EGL_NO_SURFACE) {
            log->error("Error creating EGL window surface: {}", eglGetErrorStr());
            eglDestroyContext(_display, _context);
            eglTerminate(_display);
            gbm_reset();
        }

        eglMakeCurrent(_display, _surface, _surface, _context);
        gladLoadEGL();
    }

    EGLConfig graphics_device_drm::find_matching_egl_config() {
        EGLint config_count{};
        eglGetConfigs(_display, nullptr, 0, &config_count);

        std::vector<EGLConfig> configs(config_count);
        if (!eglChooseConfig(_display, config_attribs, configs.data(), configs.size(), &config_count)) {
            log->error("Error getting EGL configurations: {}", eglGetErrorStr());
            eglTerminate(_display);
            gbm_reset();
            throw std::runtime_error{"Error getting EGL configuration"};
        }

        EGLConfig matching_config = nullptr;
        for (auto i = 0; i < config_count; ++i) {
            const auto conf = configs[i];
            EGLint id;

            if (!eglGetConfigAttrib(_display, conf, EGL_NATIVE_VISUAL_ID, &id)) {
                continue;
            }

            if (id == GBM_FORMAT_XRGB8888) {
                matching_config = conf;
                break;
            }
        }

        if (!matching_config) {
            log->error("Could not find matching EGL config for XRGB8888 mode");
            eglTerminate(_display);
            gbm_reset();
            throw std::runtime_error{"No matching config found"};
        }

        return matching_config;
    }
}

