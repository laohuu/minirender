#ifndef RAYTRACING_SHADER_H
#define RAYTRACING_SHADER_H

#include "iostream"
#include "vector"
#include "glm/glm.hpp"
#include "utils.h"
#include "vertex.h"
#include "framebuffer.h"
#include "material.h"
#include "vertex2fragment.h"


class shader {
public:
    shader() {
        model_matrix = glm::mat4(1.0f);
        view_matrix = glm::mat4(1.0f);
        projection_matrix = glm::mat4(1.0f);
        _material = nullptr;
    }

    ~shader() = default;

private:
    glm::mat4 model_matrix{};
    glm::mat4 view_matrix{};
    glm::mat4 projection_matrix{};
    shared_ptr<material> _material;

public:
    void homogeneous_division(glm::vec4 &project_pos) {
        project_pos.x = project_pos.x / project_pos.w;
        project_pos.y = project_pos.y / project_pos.w;
        project_pos.z = project_pos.z / project_pos.w;
    }

    vertex2fragment vertex_shader(const vertex &a2v) {
        vertex2fragment v2f;
        v2f.world_pos = model_matrix * a2v.position;
        v2f.view_pos = view_matrix * v2f.world_pos;
        v2f.projection_pos = projection_matrix * view_matrix * model_matrix * a2v.position;
        v2f.color = a2v.color;
        v2f.normal = glm::transpose(glm::inverse(projection_matrix * view_matrix)) * glm::vec4(a2v.normal, 1.0f);
        v2f.texcoord = a2v.texcoord;
        return v2f;
    }

    glm::vec4 fragment_shader(const vertex2fragment &v2f) {

        if (_material != nullptr) {
            //repeate
            float u = v2f.texcoord.x - std::floor(v2f.texcoord.x);
            float v = v2f.texcoord.y - std::floor(v2f.texcoord.y);

            return _material->get_diffuse(u, v);
        }

        glm::vec4 color;
        color = v2f.color;
        return color;
    }

    void set_model_matrix(const glm::mat4 &model) {
        model_matrix = model;
    }

    void set_material(shared_ptr<material> _mat) {
        _material = _mat;
    }

    void set_view_matrix(const glm::mat4 &view) {
        view_matrix = view;
    }

    void set_projection_matrix(const glm::mat4 &project) {
        projection_matrix = project;
    }


};

#endif //RAYTRACING_SHADER_H
