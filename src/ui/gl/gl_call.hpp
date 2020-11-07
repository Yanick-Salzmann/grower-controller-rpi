#ifndef GROWER_CONTROLLER_RPI_GL_CALL_HPP
#define GROWER_CONTROLLER_RPI_GL_CALL_HPP

#define GL_CALL(call) { \
    glGetError(); \
    call;\
    const auto error = glGetError(); \
    if(error) { log->error("Call {} failed with error: {}", #call, error); throw std::runtime_error("GL call failed: " #call); } \
    } ((void) 0)

#endif //GROWER_CONTROLLER_RPI_GL_CALL_HPP
