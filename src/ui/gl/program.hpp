#ifndef GROWER_CONTROLLER_RPI_PROGRAM_HPP
#define GROWER_CONTROLLER_RPI_PROGRAM_HPP

#include <GLES3/gl3.h>
#include "glad_egl.h"
#include <string>
#include <cstdint>
#include <memory>
#undef countof
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "utils/log.hpp"

namespace grower::ui::gl {
    class program {
        LOGGER;

        GLuint _vertex_shader;
        GLuint _fragment_shader;
        GLuint _program;

        static void compile_shader(GLuint shader, const std::string& source);

    public:
        program();

        program& compile_vertex_shader(const std::string& source) {
            compile_shader(_vertex_shader, source);
            return *this;
        }

        program& compile_fragment_shader(const std::string& source) {
            compile_shader(_fragment_shader, source);
            return *this;
        }

        program& link();

        program& use();

        int32_t uniform_location(const std::string& name);
        int32_t attribute_location(const std::string& name);

#define UNIFORM(type, name) \
        program& set_##name(const std::string& uniform_name, const type& value) { \
            return set_##name(uniform_location(uniform_name), value); \
        } \
        \
        program& set_##name(int32_t index, const type& value)

#define PROLOG \
        if(index < 0) { \
            throw std::runtime_error("Error setting uniform, index not found"); \
        } \
        use()

        UNIFORM(float, float) {
            PROLOG;
            glUniform1f(index, value);
            return *this;
        }

        UNIFORM(glm::vec4, vec4) {
            PROLOG;
            glUniform4fv(index, 1, glm::value_ptr(value));
            return *this;
        }

        UNIFORM(glm::vec3, vec3) {
            PROLOG;
            glUniform3fv(index, 1, glm::value_ptr(value));
            return *this;
        }

        UNIFORM(glm::vec2, vec2) {
            PROLOG;
            glUniform2fv(index, 1, glm::value_ptr(value));
            return *this;
        }

        UNIFORM(int, int) {
            PROLOG;
            glUniform1i(index, value);
            return *this;
        }

        UNIFORM(glm::mat4, matrix) {
            PROLOG;
            glUniformMatrix4fv(index, 1, GL_FALSE, glm::value_ptr(value));
            return *this;
        }
    };

    typedef std::shared_ptr<program> program_ptr;
}

#endif //GROWER_CONTROLLER_RPI_PROGRAM_HPP
