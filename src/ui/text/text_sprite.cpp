#include "text_sprite.hpp"
#include "fontconfig/fontconfig.h"
#include <png++/png.hpp>

namespace grower::ui::text {
    gl::mesh_ptr text_sprite::_mesh;
    gl::program_ptr text_sprite::_program;

    text_sprite::text_sprite(std::string font_name) : _font_name{std::move(font_name)} {
        _texture = std::make_shared<gl::texture>();
        _vertex_buffer = std::make_shared<gl::vertex_buffer>();
    }

    text_sprite &text_sprite::text(const std::string &text) {
        _text = text;
        update();
        return *this;
    }

    void text_sprite::update() {
        if (_text.empty()) {
            _dimension = {};
            return;
        }

        unique_ptr_del<cairo_surface_t> surface(cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 1, 1));
        auto *st = surface.get();

        unique_ptr_del<cairo_t> context(cairo_create(st));
        auto *ct = context.get();

        cairo_set_antialias(ct, cairo_antialias_t::CAIRO_ANTIALIAS_BEST);

        unique_ptr_del<PangoLayout> layout(pango_cairo_create_layout(ct));
        auto *lt = layout.get();

        pango_layout_set_text(lt, _text.c_str(), static_cast<int>(_text.size()));

        unique_ptr_del<PangoFontDescription> font_description(pango_font_description_new());
        auto *fd = font_description.get();

        pango_font_description_set_family(fd, _font_name.c_str());
        pango_font_description_set_weight(fd, PANGO_WEIGHT_BOLD);
        pango_font_description_set_absolute_size(fd, _size * PANGO_SCALE);

        pango_layout_set_font_description(lt, fd);

        PangoRectangle text_rect{};
        pango_layout_get_pixel_extents(lt, nullptr, &text_rect);

        const auto total_width = text_rect.width + std::abs(text_rect.x);
        const auto total_height = text_rect.height + std::abs(text_rect.y);

        layout.reset();
        context.reset();

        surface.reset(cairo_image_surface_create(CAIRO_FORMAT_ARGB32, total_width, total_height));
        st = surface.get();

        context.reset(cairo_create(st));
        ct = context.get();

        layout.reset(pango_cairo_create_layout(ct));
        lt = layout.get();

        pango_layout_set_text(lt, _text.c_str(), static_cast<int>(_text.size()));
        pango_layout_set_font_description(lt, fd);

        cairo_set_source_rgba(ct, 0.0f, 0.0f, 0.0f, 0.0f);
        cairo_rectangle(ct, 0.0, 0.0, total_width, total_height);
        cairo_fill(ct);
        cairo_set_source_rgba(ct, _color.r, _color.g, _color.b, _color.a);
        cairo_move_to(ct, text_rect.x, text_rect.y);
        pango_cairo_show_layout(ct, lt);

        layout.reset();
        context.reset();

        const auto stride = cairo_image_surface_get_stride(st);
        const auto ptr = cairo_image_surface_get_data(st);

        if (stride == total_width * 4) {
            _texture->load_from_memory_argb(total_width, total_height, ptr, total_width * total_height * 4);
        } else {
            std::vector<uint8_t> clamped_data(total_width * total_height * 4);
            for (auto y = 0; y < total_height; ++y) {
                memcpy(clamped_data.data() + y * total_width * 4, ptr + y * stride, total_width * 4);
            }
            _texture->load_from_memory_argb(total_width, total_height, clamped_data);
        }

        _dimension = glm::vec2{(float) total_width, (float) total_height};

        update_vertices();
    }

    text_sprite &text_sprite::color(glm::vec4 color) {
        _color = color;
        update();
        return *this;
    }

    void text_sprite::init_mesh() {
        _program = std::make_shared<gl::program>();
        _program->compile_vertex_shader(R"(
attribute vec2 position0;
attribute vec2 texCoord0;

varying vec2 texCoord;

uniform mat4 ortho;

void main() {
    texCoord = texCoord0;
    gl_Position = ortho * vec4(position0, 0.0, 1.0);
}
)");
        _program->compile_fragment_shader(R"(
varying vec2 texCoord;

uniform sampler2D textTexture;
uniform vec2 dimension;

vec2 ofs_tl = vec2(-1.0 / dimension.x, 0.0f);
vec2 ofs_tr = vec2(1.0 / dimension.x, 0.0f);
vec2 ofs_br = vec2(0.0f, 1.0 / dimension.y);
vec2 ofs_bl = vec2(0.0f, -1.0 / dimension.y);

void main() {
    vec4 c0 = texture2D(textTexture, texCoord);
    vec4 c1 = texture2D(textTexture, texCoord + ofs_tl);
    vec4 c2 = texture2D(textTexture, texCoord + ofs_tr);
    vec4 c3 = texture2D(textTexture, texCoord + ofs_br);
    vec4 c4 = texture2D(textTexture, texCoord + ofs_bl);

    gl_FragColor = vec4(c0.rgb, (c0.a * 2.0 + c1.a + c2.a + c3.a + c4.a) / 6.0);
}
)");

        _program->link();
        _program->set_int("textTexture", 0);

        _mesh = std::make_shared<gl::mesh>();
        _mesh->program(_program)
                .index_count(6)
                .add_element("position", 0, 2)
                .add_element("texCoord", 0, 2)
                .finalize();

        _mesh->blend().mode(gl::blend_mode::alpha);

        _mesh->index_buffer()->data<uint16_t>({0, 1, 2, 0, 2, 3});
    }

    void text_sprite::update_ortho_matrix(const glm::mat4 &matrix) {
        _program->set_matrix("ortho", matrix);
    }

    void text_sprite::render() {
        if (_text.empty()) {
            return;
        }

        _program->set_vec2("dimension", _dimension);

        glActiveTexture(GL_TEXTURE0);
        _texture->bind();
        _mesh->vertex_buffer(_vertex_buffer);

        _mesh->begin_draw();
        _mesh->draw();
    }

    void text_sprite::update_vertices() {
        _vertex_buffer->data<float>(
                std::vector{
                        (float) (int32_t) _position.x, (float) (int32_t) _position.y, 0.0f, 0.0f,
                        (float) (int32_t) (_position.x + _dimension.x), (float) (int32_t) _position.y, 1.0f, 0.0f,
                        (float) (int32_t) (_position.x + _dimension.x), (float) (int32_t) (_position.y + _dimension.y), 1.0f, 1.0f,
                        (float) (int32_t) _position.x, (float) (int32_t) (_position.y + _dimension.y), 0.0f, 1.0f
                }
        );
    }

    glm::vec2 text_sprite::position() {
        return _position;
    }

    glm::vec2 text_sprite::size() {
        return _dimension;
    }
}