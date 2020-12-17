cmake_minimum_required(VERSION ${REQUIRED_CMAKE_VERSION})
project(third_party)

include(ExternalProject)

set(SPDLOG_ROOT ${CMAKE_CURRENT_BINARY_DIR}/spdlog)
set(HTTP_ROOT ${CMAKE_CURRENT_BINARY_DIR}/cpp-httplib)
set(JSON_ROOT ${CMAKE_CURRENT_BINARY_DIR}/json)
set(TOML_ROOT ${CMAKE_CURRENT_BINARY_DIR}/toml)

# https://github.com/gabime/spdlog/archive/v1.8.1.tar.gz
# https://github.com/yhirose/cpp-httplib/archive/v0.7.8.tar.gz
# https://github.com/nlohmann/json/archive/v3.9.1.tar.gz
# https://github.com/ToruNiina/toml11/archive/v3.6.0.tar.gz

ExternalProject_Add(
        spdlog
        PREFIX ${SPDLOG_ROOT}
        URL https://github.com/gabime/spdlog/archive/v1.8.1.tar.gz
        SOURCE_DIR ${SPDLOG_ROOT}/src/spdlog
        INSTALL_DIR ${SPDLOG_ROOT}
        CMAKE_CACHE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/spdlog
        -DSPDLOG_BUILD_SHARED:BOOL=OFF
        -DSPDLOG_BUILD_TESTS:BOOL=OFF
        -DSPDLOG_BUILD_TESTS_HO:BOOL=OFF
        -DSPDLOG_BUILD_BENCH:BOOL=OFF
        -DSPDLOG_BUILD_TESTING:BOOL=OFF
        "-DCMAKE_CXX_FLAGS:STRING=-fpermissive"
        -DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}
        -DCMAKE_C_COMPILER:STRING=${CMAKE_C_COMPILER}
        -DSPDLOG_BUILD_EXAMPLE:BOOL=OFF
        -DCMAKE_VERBOSE_MAKEFILE:BOOL=ON
)

ExternalProject_Add(
        cpp-httplib
        PREFIX ${HTTP_ROOT}
        URL https://github.com/yhirose/cpp-httplib/archive/v0.7.8.tar.gz
        SOURCE_DIR ${HTTP_ROOT}/src/cpp-httplib
        INSTALL_DIR ${HTTP_ROOT}
        CMAKE_CACHE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${HTTP_ROOT}
        -DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}
        -DCMAKE_C_COMPILER:STRING=${CMAKE_C_COMPILER}
)

ExternalProject_Add(
        toml
        PREFIX ${TOML_ROOT}
        URL https://github.com/ToruNiina/toml11/archive/v3.6.0.tar.gz
        SOURCE_DIR ${TOML_ROOT}/src/cpp-httplib
        INSTALL_DIR ${TOML_ROOT}
        CMAKE_CACHE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${TOML_ROOT}
        -DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}
        -DCMAKE_C_COMPILER:STRING=${CMAKE_C_COMPILER}
)

ExternalProject_Add(
        json
        PREFIX ${JSON_ROOT}
        URL https://github.com/nlohmann/json/archive/v3.9.1.tar.gz
        SOURCE_DIR ${JSON_ROOT}/src/json
        INSTALL_DIR ${JSON_ROOT}
        CMAKE_CACHE_ARGS
        -DCMAKE_INSTALL_PREFIX:PATH=${JSON_ROOT}
        -DCMAKE_CXX_COMPILER:STRING=${CMAKE_CXX_COMPILER}
        -DCMAKE_C_COMPILER:STRING=${CMAKE_C_COMPILER}
        -DJSON_BuildTests:BOOL=OFF
)