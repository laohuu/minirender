#ifndef RAYTRACING_RASTERIZER_H
#define RAYTRACING_RASTERIZER_H

#include "framebuffer.h"
#include "shader.h"
#include "aabb.h"

class rasterizer {

private:
    int width;
    int height;
    int channel;
    framebuffer *frame_buffer;
    shader *render;
    glm::mat4 viewport_matrix;
public:
    rasterizer(const int &w, const int &h, const int &c) :
            width(w), height(h), channel(c), frame_buffer(nullptr), render(nullptr) {
        init();
    }

    ~rasterizer() {
        if (frame_buffer)
            delete frame_buffer;
        if (render)
            delete render;

        frame_buffer = nullptr;
        render = nullptr;
    }

    void set_texture(shared_ptr<texture> _tex) {
        render->set_texture(_tex);
    }

    void set_model_matrix(const glm::mat4 &model) {
        render->set_model_matrix(model);
    }

    void set_view_matrix(const glm::mat4 &view) {
        render->set_view_matrix(view);
    }

    void set_projection_matrix(const glm::mat4 &project) {
        render->set_projection_matrix(project);
    }

    void init() {
        if (frame_buffer)
            delete frame_buffer;
        if (render)
            delete render;
        viewport_matrix = get_viewport_matrix();
        frame_buffer = new framebuffer(width, height, channel);
        render = new shader();
    }

    void resize(const int &w, const int &h) {
        width = w;
        height = h;
        init();
    }

    void clear_color_buffer(const glm::vec4 &color) {
        frame_buffer->clear_color_buffer(color);
    }

    void ouput_image() {
        stbi_flip_vertically_on_write(true);
//        stbi_write_png(FileSystem::getPath("images/render_triangle.png").c_str(), width, height, channel,
//                       frame_buffer->buffer_data, 0);
        stbi_write_png(FileSystem::getPath("test.png").c_str(), width, height, channel,
                       frame_buffer->buffer_data, 0);
    }

    glm::mat4 get_viewport_matrix() {
        glm::mat4 result = glm::mat4(1.0f);
        result[0][0] = width / 2.0f;
        result[3][0] = width / 2.0f;
        result[1][1] = height / 2.0f;
        result[3][1] = height / 2.0f;
        return result;
    }

    bool face_culling(const glm::vec4 &v1, const glm::vec4 &v2, const glm::vec4 &v3) {
        glm::vec3 tmp1 = glm::vec3(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z);
        glm::vec3 tmp2 = glm::vec3(v3.x - v1.x, v3.y - v1.y, v3.z - v1.z);
        glm::vec3 normal = glm::normalize(glm::cross(tmp1, tmp2));
        glm::vec3 view = glm::vec3(0, 0, 1);
        return glm::dot(normal, view) < 0;
    }

    void viewport_transformation(vertex2fragment &v2f) {
        v2f.viewport_pos =
                viewport_matrix * glm::vec4(v2f.projection_pos.x, v2f.projection_pos.y, v2f.projection_pos.z, 1.0f);
    }

    std::tuple<float, float, float>
    compute_barycentric2D(float x, float y, glm::vec4 vert1, glm::vec4 vert2, glm::vec4 vert3) {
        float c1 = (x * (vert2.y - vert3.y) + (vert3.x - vert2.x) * y + vert2.x * vert3.y - vert3.x * vert2.y) /
                   (vert1.x * (vert2.y - vert3.y) + (vert3.x - vert2.x) * vert1.y + vert2.x * vert3.y -
                    vert3.x * vert2.y);
        float c2 = (x * (vert3.y - vert1.y) + (vert1.x - vert3.x) * y + vert3.x * vert1.y - vert1.x * vert3.y) /
                   (vert2.x * (vert3.y - vert1.y) + (vert1.x - vert3.x) * vert2.y + vert3.x * vert1.y -
                    vert1.x * vert3.y);
        float c3 = (x * (vert1.y - vert2.y) + (vert2.x - vert1.x) * y + vert1.x * vert2.y - vert2.x * vert1.y) /
                   (vert3.x * (vert1.y - vert2.y) + (vert2.x - vert1.x) * vert3.y + vert1.x * vert2.y -
                    vert2.x * vert1.y);
        return {c1, c2, c3};
    }

    double product(glm::vec2 p1, glm::vec2 p2, glm::vec2 p3) {
        return (p2.x - p1.x) * (p3.y - p1.y) - (p2.y - p1.y) * (p3.x - p1.x);
    }

    bool inside_triangle(float x, float y, const glm::vec4 &vert1, const glm::vec4 &vert2, const glm::vec4 &vert3) {
        glm::vec2 vert(x, y);
        glm::vec2 p1(vert1.x, vert1.y);
        glm::vec2 p2(vert2.x, vert2.y);
        glm::vec2 p3(vert3.x, vert3.y);
        //保证p1，p2，p3是逆时针顺序
        if (product(p1, p2, p3) < 0) return inside_triangle(x, y, vert1, vert3, vert2);
        if (product(p1, p2, vert) > 0 && product(p2, p3, vert) > 0 && product(p3, p1, vert) > 0)
            return true;
        return false;
    }

    void get_viewport_index(glm::vec4 viewport_pos, glm::ivec2 &viewport_index) {
        auto i = static_cast<int>(viewport_pos.x);
        auto j = static_cast<int>(viewport_pos.y);

        if (i < 0) i = 0;
        if (j < 0) j = 0;
        if (i >= width) i = width - 1;
        if (j >= height) j = height - 1;
        viewport_index.x = i;
        viewport_index.y = j;
    }

    void render_point(const vertex &vert) {
        vertex2fragment v2f = render->vertex_shader(vert);
        render->homogeneous_division(v2f.projection_pos);
        v2f.viewport_pos = viewport_matrix * v2f.projection_pos;
        glm::ivec2 viewport_index(0);
        get_viewport_index(v2f.viewport_pos, viewport_index);
        frame_buffer->set_pixel(viewport_index.x, viewport_index.y, vert.color);
    }

    void render_line(const vertex &start_vert, const vertex &end_vert) {
        vertex2fragment v2f_start = render->vertex_shader(start_vert);
        render->homogeneous_division(v2f_start.projection_pos);
        v2f_start.viewport_pos = viewport_matrix * v2f_start.projection_pos;
        vertex2fragment v2f_end = render->vertex_shader(end_vert);
        render->homogeneous_division(v2f_end.projection_pos);
        v2f_end.viewport_pos = viewport_matrix * v2f_end.projection_pos;
        glm::ivec2 start_index;
        get_viewport_index(v2f_start.viewport_pos, start_index);
        glm::ivec2 end_index;
        get_viewport_index(v2f_end.viewport_pos, end_index);
        int x0 = start_index.x;
        int y0 = start_index.y;
        int x1 = end_index.x;
        int y1 = end_index.y;
        bool steep = false;
        if (std::abs(x0 - x1) < std::abs(y0 - y1)) {
            std::swap(x0, y0);
            std::swap(x1, y1);
            steep = true;
        }
        if (x0 > x1) {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }
        int dx = x1 - x0;
        int dy = y1 - y0;
        int derror2 = std::abs(dy) * 2;
        int error2 = 0;
        int y = y0;
        for (int x = x0; x <= x1; x++) {
            if (steep) {
                frame_buffer->set_pixel(y, x, start_vert.color);
            } else {
                frame_buffer->set_pixel(x, y, start_vert.color);
            }
            error2 += derror2;
            if (error2 > dx) {
                y += (y1 > y0 ? 1 : -1);
                error2 -= dx * 2;
            }
        }
    }

    void render_triangle(const vertex &v1, const vertex &v2, const vertex &v3) {
        vertex2fragment o1 = render->vertex_shader(v1);
        vertex2fragment o2 = render->vertex_shader(v2);
        vertex2fragment o3 = render->vertex_shader(v3);

        //ClipTriangle

        render->homogeneous_division(o1.projection_pos);
        render->homogeneous_division(o2.projection_pos);
        render->homogeneous_division(o3.projection_pos);

//        if (o1.projection_pos.x < -1.0f || o1.projection_pos.x > 1.0f ||
//            o2.projection_pos.x < -1.0f || o2.projection_pos.x > 1.0f ||
//            o3.projection_pos.x < -1.0f || o3.projection_pos.x > 1.0f) {
//            std::cout << "delete" << std::endl;
//            return;
//        }

        if (face_culling(o1.projection_pos, o2.projection_pos, o3.projection_pos)) {
            return;
        }


        viewport_transformation(o1);
        viewport_transformation(o2);
        viewport_transformation(o3);

        float minx = std::min(o1.viewport_pos.x, std::min(o2.viewport_pos.x, o3.viewport_pos.x));
        float miny = std::min(o1.viewport_pos.y, std::min(o2.viewport_pos.y, o3.viewport_pos.y));
        float maxx = std::max(o1.viewport_pos.x, std::max(o2.viewport_pos.x, o3.viewport_pos.x));
        float maxy = std::max(o1.viewport_pos.y, std::max(o2.viewport_pos.y, o3.viewport_pos.y));

        int min_x = floor(minx);
        int min_y = floor(miny);
        int max_x = ceil(maxx);
        int max_y = ceil(maxy);

        for (int i = min_x; i <= max_x; ++i) {
            for (int j = min_y; j <= max_y; ++j) {
//                if (!inside_triangle(i + 0.5, j + 0.5, o1.viewport_pos, o2.viewport_pos, o3.viewport_pos))
//                    continue;
                auto [alpha, beta, gamma] = compute_barycentric2D(i + 0.5, j + 0.5, o1.viewport_pos, o2.viewport_pos,
                                                                  o3.viewport_pos);
                if (!(alpha >= 0 && beta >= 0 && gamma >= 0))
                    continue;

                float Z = 1.0f / (alpha / o1.projection_pos.w + beta / o2.projection_pos.w +
                                  gamma / o3.projection_pos.w);
                auto zp = interpolate(alpha, beta, gamma, o1.projection_pos.w, o2.projection_pos.w,
                                      o3.projection_pos.w, o1.viewport_pos.z, o2.viewport_pos.z, o3.viewport_pos.z, Z);

                float buf_depth = frame_buffer->get_depth(i, j);
                if (zp >= buf_depth) continue;
                frame_buffer->write_depth(i, j, zp);

                auto interpolated_color = interpolate(alpha, beta, gamma, o1.projection_pos.w, o2.projection_pos.w,
                                                      o3.projection_pos.w, o1.color, o2.color, o3.color, Z);
                auto interpolated_normal = interpolate(alpha, beta, gamma, o1.projection_pos.w, o2.projection_pos.w,
                                                       o3.projection_pos.w, o1.normal, o2.normal, o3.normal, Z);
                auto interpolated_texcoord = interpolate(alpha, beta, gamma, o1.projection_pos.w, o2.projection_pos.w,
                                                         o3.projection_pos.w, o1.texcoord, o2.texcoord, o3.texcoord, Z);
                auto interpolated_projection_pos = interpolate(alpha, beta, gamma, o1.projection_pos.w,
                                                               o2.projection_pos.w,
                                                               o3.projection_pos.w, o1.projection_pos,
                                                               o2.projection_pos,
                                                               o3.projection_pos, Z);
                auto interpolated_world_pos = interpolate(alpha, beta, gamma, o1.projection_pos.w, o2.projection_pos.w,
                                                          o3.projection_pos.w, o1.world_pos, o2.world_pos,
                                                          o3.world_pos, Z);

                vertex2fragment v2f(interpolated_world_pos, interpolated_projection_pos, interpolated_color,
                                    interpolated_texcoord, interpolated_normal);
                auto color = render->fragment_shader(v2f);
                frame_buffer->set_pixel(i, j, color);
            }
        }
    }

};

#endif //RAYTRACING_RASTERIZER_H
