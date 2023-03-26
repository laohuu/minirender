#ifndef RAYTRACING_RASTERIZER_H
#define RAYTRACING_RASTERIZER_H

#include "framebuffer.h"
#include "shader.h"

class rasterizer {

private:
    int width;
    int height;
    int channel;
    framebuffer *frame_buffer;
    shared_ptr<shader> render;
    glm::mat4 viewport_matrix;


public:
    rasterizer(const int &w, const int &h, const int &c) :
            width(w), height(h), channel(c), frame_buffer(nullptr), render(nullptr) {
        init();
    }

    rasterizer(const int &w, const int &h, const int &c, shared_ptr<shader> _shader) :
            width(w), height(h), channel(c), frame_buffer(nullptr), render(_shader) {
        viewport_matrix = get_viewport_matrix();
        frame_buffer = new framebuffer(width, height, channel);
    }

    ~rasterizer() {
        if (frame_buffer)
            delete frame_buffer;

        frame_buffer = nullptr;
        render = nullptr;
    }

    void set_material(shared_ptr<material> _material) {
        render->set_material(_material);
    }

    void set_model_matrix(const glm::mat4 &model) {
        render->set_model_matrix(model);
    }

    void set_view_matrix(const glm::mat4 &view) {
        render->set_view_matrix(view);
    }

    void set_camera_pos(const glm::vec3 &pos) {
        render->set_camera_pos(pos);
    }

    void push_dir_light(shared_ptr<direction_light> dir_lig) {
        render->push_dir_light(dir_lig);
    }

    void push_spot_light(shared_ptr<spot_light> spot_lig) {
        render->push_spot_light(spot_lig);
    }

    void push_point_light(shared_ptr<point_light> point_lig) {
        render->push_point_light(point_lig);
    }

    void set_projection_matrix(const glm::mat4 &project) {
        render->set_projection_matrix(project);
    }

    void init() {
        if (frame_buffer)
            delete frame_buffer;
        viewport_matrix = get_viewport_matrix();
        frame_buffer = new framebuffer(width, height, channel);
        render = make_shared<shader>();
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

    glm::ivec2 get_viewport_index(glm::vec4 viewport_pos) {
        auto i = static_cast<int>(viewport_pos.x + 0.5);
        auto j = static_cast<int>(viewport_pos.y + 0.5);
        return {i, j};
    }

    bool is_inside_plane(const glm::vec4 &project_plane, const glm::vec4 &p) {
        return project_plane.x * p.x + project_plane.y * p.y + project_plane.z * p.z + project_plane.w * p.w >= 0;
    }

    vertex2fragment get_interact_fragment(const vertex2fragment &v1,
                                          const vertex2fragment &v2,
                                          const glm::vec4 &project_plane) {
        float da = v1.projection_pos.x * project_plane.x + v1.projection_pos.y * project_plane.y +
                   v1.projection_pos.z * project_plane.z +
                   project_plane.w * v1.projection_pos.w;
        float db = v2.projection_pos.x * project_plane.x + v2.projection_pos.y * project_plane.y +
                   v2.projection_pos.z * project_plane.z +
                   project_plane.w * v2.projection_pos.w;
        float weight = da / (da - db);
        return vertex2fragment::lerp(v1, v2, weight);
    }

    bool all_vertexs_inside(glm::vec4 vec1, glm::vec4 vec2, glm::vec4 vec3) {
        if (vec1.x > vec1.w || vec1.x < -vec1.w)
            return false;
        if (vec1.y > vec1.w || vec1.y < -vec1.w)
            return false;
        if (vec2.x > vec1.w || vec2.x < -vec1.w)
            return false;
        if (vec2.y > vec1.w || vec2.y < -vec1.w)
            return false;
        if (vec3.x > vec1.w || vec3.x < -vec1.w)
            return false;
        if (vec3.y > vec1.w || vec3.y < -vec1.w)
            return false;
        return true;
    }

    std::vector<vertex2fragment> sutherland_hodgeman(const vertex2fragment &v1,
                                                     const vertex2fragment &v2,
                                                     const vertex2fragment &v3) {
        std::vector<vertex2fragment> output_fragment = {v1, v2, v3};
        if (all_vertexs_inside(v1.projection_pos, v2.projection_pos, v3.projection_pos)) {
            return output_fragment;
        }
        const std::vector<glm::vec4> view_planes = {
                //near
                glm::vec4(0, 0, 1, 1),
                //far
                glm::vec4(0, 0, -1, 1),
                //left
                glm::vec4(1, 0, 0, 1),
                //right
                glm::vec4(-1, 0, 0, 1),
                //top
                glm::vec4(0, -1, 0, 1),
                //bottom
                glm::vec4(0, 1, 0, 1)
        };

        for (auto plane: view_planes) {
            std::vector<vertex2fragment> input_fragment(output_fragment);
            output_fragment.clear();
            int input_fragment_len = input_fragment.size();
            for (int i = 0; i < input_fragment_len; ++i) {
                auto p_fragment = input_fragment[i];
                auto s_fragment = input_fragment[(i + input_fragment_len - 1) % input_fragment_len];
                if (is_inside_plane(plane, p_fragment.projection_pos)) {
                    if (!is_inside_plane(plane, s_fragment.projection_pos)) {
                        auto new_fragment = get_interact_fragment(p_fragment, s_fragment, plane);
                        output_fragment.push_back(new_fragment);
                    }
                    output_fragment.push_back(p_fragment);
                } else if (is_inside_plane(plane, s_fragment.projection_pos)) {
                    auto new_fragment = get_interact_fragment(p_fragment, s_fragment, plane);
                    output_fragment.push_back(new_fragment);
                }
            }
        }
        return output_fragment;
    }

    void render_point(const vertex &vert) {
        vertex2fragment v2f = render->vertex_shader(vert);
        render->homogeneous_division(v2f.projection_pos);
        v2f.viewport_pos = viewport_matrix * v2f.projection_pos;
        glm::ivec2 viewport_index = get_viewport_index(v2f.viewport_pos);
        frame_buffer->set_pixel(viewport_index.x, viewport_index.y, vert.color);
    }

    void render_line(const vertex &start_vert, const vertex &end_vert) {
        vertex2fragment v2f_start = render->vertex_shader(start_vert);
        vertex2fragment v2f_end = render->vertex_shader(end_vert);

        render->homogeneous_division(v2f_start.projection_pos);
        render->homogeneous_division(v2f_end.projection_pos);

        viewport_transformation(v2f_start);
        viewport_transformation(v2f_end);

        glm::ivec2 start_index = get_viewport_index(v2f_start.viewport_pos);
        glm::ivec2 end_index = get_viewport_index(v2f_end.viewport_pos);
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
                frame_buffer->set_pixel(y, x, glm::vec4(1.0f));
            } else {
                frame_buffer->set_pixel(x, y, glm::vec4(1.0f));
            }
            error2 += derror2;
            if (error2 > dx) {
                y += (y1 > y0 ? 1 : -1);
                error2 -= dx * 2;
            }
        }
    }

    void render_viewport_line(const glm::vec4 &start_viewport_pos, const glm::vec4 &end_viewport_pos) {
        glm::ivec2 start_index = get_viewport_index(start_viewport_pos);
        glm::ivec2 end_index = get_viewport_index(end_viewport_pos);
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
                frame_buffer->set_pixel(y, x, glm::vec4(1.0f));
            } else {
                frame_buffer->set_pixel(x, y, glm::vec4(1.0f));
            }
            error2 += derror2;
            if (error2 > dx) {
                y += (y1 > y0 ? 1 : -1);
                error2 -= dx * 2;
            }
        }
    }

    void wireframe_triangle(const vertex &v1, const vertex &v2, const vertex &v3) {
        vertex2fragment o1 = render->vertex_shader(v1);
        vertex2fragment o2 = render->vertex_shader(v2);
        vertex2fragment o3 = render->vertex_shader(v3);

        //Clip Triangle
        std::vector<vertex2fragment> clip_triangles = sutherland_hodgeman(o1, o2, o3);
        int len = clip_triangles.size() - 3 + 1;
        for (int i = 0; i < len; i++) {
            o1 = clip_triangles[0];
            o2 = clip_triangles[i + 1];
            o3 = clip_triangles[i + 2];

            render->homogeneous_division(o1.projection_pos);
            render->homogeneous_division(o2.projection_pos);
            render->homogeneous_division(o3.projection_pos);

            viewport_transformation(o1);
            viewport_transformation(o2);
            viewport_transformation(o3);

            render_viewport_line(o1.viewport_pos, o2.viewport_pos);
            render_viewport_line(o2.viewport_pos, o3.viewport_pos);
            render_viewport_line(o1.viewport_pos, o3.viewport_pos);
        }
    }

    void render_fragment_triangle(vertex2fragment &o1, vertex2fragment &o2, vertex2fragment &o3) {
        render->homogeneous_division(o1.projection_pos);
        render->homogeneous_division(o2.projection_pos);
        render->homogeneous_division(o3.projection_pos);

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

    void render_triangle(const vertex &v1, const vertex &v2, const vertex &v3) {
        vertex2fragment o1 = render->vertex_shader(v1);
        vertex2fragment o2 = render->vertex_shader(v2);
        vertex2fragment o3 = render->vertex_shader(v3);

        //Clip Triangle
        std::vector<vertex2fragment> clip_triangles = sutherland_hodgeman(o1, o2, o3);
        int len = clip_triangles.size() - 3 + 1;
        for (int i = 0; i < len; i++) {
            o1 = clip_triangles[0];
            o2 = clip_triangles[i + 1];
            o3 = clip_triangles[i + 2];
            render_fragment_triangle(o1, o2, o3);
        }
    }
};

#endif //RAYTRACING_RASTERIZER_H
