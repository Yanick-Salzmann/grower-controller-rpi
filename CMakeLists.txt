set(REQUIRED_CMAKE_VERSION 3.12)

cmake_minimum_required(VERSION ${REQUIRED_CMAKE_VERSION})
project(grower_controller_rpi)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/modules)

set(CMAKE_CXX_STANDARD 20)

add_compile_options(-fpermissive)

add_subdirectory(third_party)

add_compile_options(-Wno-psabi)

add_executable(grower_controller_rpi src/main.cpp src/wiring/gpio.hpp src/wiring/gpio.cpp src/wiring/wiring_context.hpp src/wiring/wiring_context.cpp src/wiring/ads1115.hpp src/wiring/ads1115.cpp src/peripheral/case_fan.hpp src/peripheral/case_fan.cpp src/utils/literals.hpp src/peripheral/power_supply.hpp src/peripheral/power_supply.cpp src/utils/log.hpp src/utils/log.cpp src/http/http_server.hpp src/http/http_server.cpp src/context.hpp src/context.cpp src/peripheral/bme280.hpp src/peripheral/bme280.cpp src/utils/error.hpp src/utils/error.cpp src/peripheral/tm1637_display.hpp src/peripheral/tm1637_display.cpp src/video/raw_camera_stream.hpp src/video/raw_camera_stream.cpp src/video/video_manager.hpp src/video/video_manager.cpp src/utils/subprocess.hpp src/utils/subprocess.cpp src/utils/string.hpp src/utils/string.cpp src/utils/singleton.hpp src/utils/singleton.cpp src/net/net_socket.hpp src/net/net_socket.cpp src/net/server_socket.hpp src/net/server_socket.cpp src/net/websocket_server.hpp src/net/websocket_server.cpp src/net/websocket_client.hpp src/net/websocket_client.cpp src/io/binary_reader.hpp src/io/binary_reader.cpp src/io/stream_reader.hpp src/io/stream_reader.cpp src/crypto/sha1.hpp src/crypto/sha1.cpp src/utils/basen.hpp src/io/byte_buffer.hpp src/io/byte_buffer.cpp src/ui/gl/graphics_device.hpp src/ui/gl/graphics_device.cpp src/ui/gl/buffers.hpp src/ui/gl/buffers.cpp src/ui/gl/glad_egl.c src/ui/gl/program.hpp src/ui/gl/program.cpp src/glm/detail/glm.cpp src/ui/gl/mesh.hpp src/ui/gl/mesh.cpp src/ui/gl/gl_call.hpp src/ui/text/font.hpp src/ui/text/font.cpp src/ui/text/text_sprite.hpp src/ui/text/text_sprite.cpp src/ui/gl/texture.hpp src/ui/gl/texture.cpp src/ui/ui_manager.hpp src/ui/ui_manager.cpp src/ui/camera_frame.hpp src/ui/camera_frame.cpp src/utils/yuv_rgb_wrapper.hpp src/utils/yuv_rgb_wrapper.cpp src/ui/elements/color_quad.hpp src/ui/elements/color_quad.cpp src/ui/elements/button.hpp src/ui/elements/button.cpp src/ui/touch_input.hpp src/ui/touch_input.cpp src/ui/elements/element.hpp src/ui/elements/element.cpp src/io/gauges.hpp src/io/gauges.cpp src/ui/text/font-awesome-icons.hpp src/ui/text/font-awesome-icons.cpp src/ui/elements/icon_button.hpp src/ui/elements/icon_button.cpp src/ui/elements/font_awesome_image.hpp src/ui/elements/font_awesome_image.cpp src/ui/views/main_view.hpp src/ui/views/main_view.cpp src/ui/views/view.hpp src/ui/views/view.cpp)

find_package(PkgConfig REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(OpenGLES2 REQUIRED)
find_package(tslib REQUIRED)

pkg_check_modules(HARFBUZZ REQUIRED harfbuzz)
pkg_check_modules(PANGO REQUIRED pango)
pkg_check_modules(PANGO_CAIRO REQUIRED pangocairo)
pkg_check_modules(FREETYPE REQUIRED freetype2)
pkg_check_modules(CAIRO REQUIRED cairo)
pkg_check_modules(PNG REQUIRED libpng)

#get_cmake_property(_variableNames VARIABLES)
#list (SORT _variableNames)
#foreach (_variableName ${_variableNames})
#    message(STATUS "${_variableName}=${${_variableName}}")
#endforeach()

target_link_libraries(
        grower_controller_rpi
        /usr/lib/libwiringPi.so
        /opt/vc/lib/libmmal.so
        /opt/vc/lib/libmmal_core.so
        /opt/vc/lib/libmmal_components.so
        /opt/vc/lib/libmmal_util.so
        /opt/vc/lib/libmmal_vc_client.so
        uuid
        Threads::Threads
        ${CMAKE_BINARY_DIR}/third_party/spdlog/lib/libspdlog.a
        ${OPENSSL_LIBRARIES}
        ${HARFBUZZ_LIBRARIES}
        ${PANGO_LIBRARIES}
        ${PANGO_CAIRO_LIBRARIES}
        ${FREETYPE_LIBRARIES}
        ${CAIRO_LIBRARIES}
        ${PNG_LIBRARIES}
        drm
        gbm
        EGL
        GLESv2
        tslib::tslib
        fontconfig
)

target_include_directories(
        grower_controller_rpi PUBLIC
        src
        /opt/vc/include
        /usr/include/libdrm
        ${CMAKE_BINARY_DIR}/third_party/spdlog/include
        ${CMAKE_BINARY_DIR}/third_party/cpp-httplib/include
        ${CMAKE_BINARY_DIR}/third_party/json/include
        ${OPENSSL_INCLUDE_DIR}
        ${HARFBUZZ_INCLUDE_DIRS}
        ${PANGO_INCLUDE_DIRS}
        ${PANGO_CAIRO_INCLUDE_DIRS}
        ${FREETYPE_INCLUDE_DIRS}
        ${CAIRO_INCLUDE_DIRS}
        ${PNG_INCLUDE_DIRS}
)

add_dependencies(grower_controller_rpi spdlog cpp-httplib json)

file(COPY web DESTINATION ${CMAKE_BINARY_DIR})
file(COPY resources DESTINATION ${CMAKE_BINARY_DIR})

add_custom_command(
        TARGET grower_controller_rpi
        POST_BUILD
        COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/web/* ${CMAKE_CURRENT_BINARY_DIR}/web
)

add_custom_command(
        TARGET grower_controller_rpi
        POST_BUILD
        COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/resources/* ${CMAKE_CURRENT_BINARY_DIR}/resources
)

add_custom_target(grower_http)
add_custom_command(
        TARGET grower_http
        POST_BUILD
        COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/web/* ${CMAKE_CURRENT_BINARY_DIR}/web
)