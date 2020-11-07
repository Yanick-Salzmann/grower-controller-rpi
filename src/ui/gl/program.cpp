#include "program.hpp"
#include "utils/string.hpp"

namespace grower::ui::gl {
    LOGGER_IMPL(program);

    program::program() {
        _vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        _fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        _program = glCreateProgram();

        glAttachShader(_program, _vertex_shader);
        glAttachShader(_program, _fragment_shader);
    }

    void program::compile_shader(GLuint shader, const std::string &source) {
        const auto src_ptr = utils::heap_copy(source);
        const auto src = src_ptr.get();

        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        GLint compile_status = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compile_status);

        if (compile_status != GL_TRUE) {
            GLint log_length = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
            std::string compile_error{"Unknown Error"};

            if (log_length > 0) {
                std::vector<char> log_info(log_length);
                glGetShaderInfoLog(shader, log_length, &log_length, log_info.data());
                if (log_length > 0) {
                    compile_error.assign(log_info.begin(), log_info.end());
                }
            }

            log->error("Error compiling shader: {}", compile_error);
        }
    }

    program &program::link() {
        glLinkProgram(_program);

        GLint link_status = GL_FALSE;
        glGetProgramiv(_program, GL_LINK_STATUS, &link_status);

        if (link_status != GL_TRUE) {
            GLint log_length = 0;
            glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &log_length);
            std::string compile_error{"Unknown Error"};

            if (log_length > 0) {
                std::vector<char> log_info(log_length);
                glGetProgramInfoLog(_program, log_length, &log_length, log_info.data());
                if (log_length > 0) {
                    compile_error.assign(log_info.begin(), log_info.end());
                }
            }

            log->error("Error linking program: {}", compile_error);
        }

        return *this;
    }

    program &program::use() {
        static GLuint active_program = -1;

        if (_program != active_program) {
            glUseProgram(_program);
            active_program = _program;
        }

        return *this;
    }

    int32_t program::uniform_location(const std::string &name) {
        return glGetUniformLocation(_program, name.c_str());
    }

    int32_t program::attribute_location(const std::string &name) {
        return glGetAttribLocation(_program, name.c_str());
    }
}