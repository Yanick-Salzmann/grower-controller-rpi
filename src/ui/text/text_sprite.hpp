#ifndef GROWER_CONTROLLER_RPI_TEXT_SPRITE_HPP
#define GROWER_CONTROLLER_RPI_TEXT_SPRITE_HPP

#include "font.hpp"
#include <cairo.h>
#include <pango/pango.h>
#include <pango/pangocairo.h>
#include "glm/vec4.hpp"
#include "ui/gl/texture.hpp"
#include "ui/gl/mesh.hpp"
#include "ui/elements/element.hpp"

namespace grower::ui::text {
    class text_sprite : public elements::element {
        struct deleter {
        public:
            void operator()(cairo_surface_t *suf) {
                cairo_surface_destroy(suf);
            }

            void operator()(cairo_t *ctx) {
                cairo_destroy(ctx);
            }

            void operator()(PangoLayout *layout) {
                g_object_unref(layout);
            }

            void operator()(PangoFontDescription *desc) {
                pango_font_description_free(desc);
            }
        };

        template<typename T>
        using unique_ptr_del = std::unique_ptr<T, deleter>;

        static gl::mesh_ptr _mesh;
        static gl::program_ptr _program;

        float _size = 30.0f;

        glm::vec2 _dimension{};
        glm::vec2 _position{};

        std::string _font_name;

        std::string _text{};
        glm::vec4 _color{1.0, 1.0, 1.0, 1.0};

        gl::texture_ptr _texture;
        gl::vertex_buffer_ptr _vertex_buffer{};

        void update();

        void update_vertices();

    public:
        explicit text_sprite(std::string font_name);

        text_sprite &text(const std::string &text);

        text_sprite &color(uint32_t rgba) {
            return color(glm::vec4{
                    (float) (rgba & 0xFFu) / 255.0f,
                    (float) ((rgba >> 8u) & 0xFFu) / 255.0f,
                    (float) ((rgba >> 16u) & 0xFFu) / 255.0f,
                    (float) ((rgba >> 24u) & 0xFFu) / 255.0f
            });
        }

        text_sprite &color(glm::vec4 color);

        text_sprite &position(float x, float y) {
            return position(glm::vec2{x, y});
        }

        text_sprite &position(glm::vec2 pos) {
            _position = pos;
            update_vertices();
            return *this;
        }

        text_sprite &font_size(float size) {
            _size = size;
            update();
            return *this;
        }

        [[nodiscard]] glm::vec2 dimension() const {
            return _dimension;
        }

        [[nodiscard]] glm::vec2 position() const {
            return _position;
        }

        glm::vec2 position() override;

        glm::vec2 size() override;

        void render() override;

        static void init_mesh();

        static void update_ortho_matrix(const glm::mat4 &matrix);
    };

    typedef std::shared_ptr<text_sprite> text_sprite_ptr;
}

#endif //GROWER_CONTROLLER_RPI_TEXT_SPRITE_HPP
