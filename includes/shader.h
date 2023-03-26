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
#include "light.h"


class shader {
public:
    shader() {
        model_matrix = glm::mat4(1.0f);
        view_matrix = glm::mat4(1.0f);
        projection_matrix = glm::mat4(1.0f);
        _material = nullptr;
        dir_lights.clear();
        point_lights.clear();
        spot_lights.clear();
    }

    ~shader() = default;

public:
    glm::mat4 model_matrix{};
    glm::mat4 view_matrix{};
    glm::mat4 projection_matrix{};
    shared_ptr<material> _material;
    glm::vec3 camera_position;

    //light
    vector<shared_ptr<direction_light>> dir_lights;
    vector<shared_ptr<point_light>> point_lights;
    vector<shared_ptr<spot_light>> spot_lights;

public:
    void homogeneous_division(glm::vec4 &project_pos) {
        project_pos.x = project_pos.x / project_pos.w;
        project_pos.y = project_pos.y / project_pos.w;
        project_pos.z = project_pos.z / project_pos.w;
    }

    virtual vertex2fragment vertex_shader(const vertex &a2v) {
        vertex2fragment v2f;
        v2f.world_pos = model_matrix * a2v.position;
        v2f.view_pos = view_matrix * v2f.world_pos;
        v2f.projection_pos = projection_matrix * view_matrix * model_matrix * a2v.position;
        v2f.color = a2v.color;
        v2f.normal = glm::mat3(glm::transpose(glm::inverse(model_matrix))) * a2v.normal;
        v2f.texcoord = a2v.texcoord;
        return v2f;
    }

    virtual glm::vec4 fragment_shader(const vertex2fragment &v2f) {
        if (_material != nullptr) {
            //repeate
            float u = v2f.texcoord.x - std::floor(v2f.texcoord.x);
            float v = v2f.texcoord.y - std::floor(v2f.texcoord.y);
            return _material->get_diffuse(u, v);
        }
        return v2f.color;
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

    void set_camera_pos(const glm::vec3 &pos) {
        camera_position = pos;
    }

    void push_dir_light(shared_ptr<direction_light> dir_lig) {
        dir_lights.push_back(dir_lig);
    }

    void push_spot_light(shared_ptr<spot_light> spot_lig) {
        spot_lights.push_back(spot_lig);
    }

    void push_point_light(shared_ptr<point_light> point_lig) {
        point_lights.push_back(point_lig);
    }
};

class blinn_phong_shader : public shader {
public:

public:
    blinn_phong_shader() {}

    ~blinn_phong_shader() = default;

    vertex2fragment vertex_shader(const vertex &a2v) override {
        vertex2fragment v2f;
        v2f.world_pos = model_matrix * a2v.position;
        v2f.view_pos = view_matrix * v2f.world_pos;
        v2f.projection_pos = projection_matrix * view_matrix * model_matrix * a2v.position;
        v2f.color = a2v.color;
        v2f.normal = glm::mat3(glm::transpose(glm::inverse(model_matrix))) * a2v.normal;
        v2f.texcoord = a2v.texcoord;
        return v2f;
    }

    glm::vec4 fragment_shader(const vertex2fragment &v2f) override {
//        glm::vec3 normal = _material->get_normal(v2f.texcoord.x, v2f.texcoord.y);
//        // transform normal vector to range [-1,1]
//        normal = glm::normalize(normal * 2.0f - 1.0f);  // this normal is in tangent space
//        return glm::vec4(v2f.normal, 1);
        glm::vec3 normal = v2f.normal;
        glm::vec3 view_dir = glm::normalize(camera_position - glm::vec3(v2f.world_pos));
        glm::vec4 result(0.f);

        for (const auto &dir_light: dir_lights) {
            result += glm::vec4(calc_dir_light(dir_light, v2f, normal, view_dir), 1.0);
        }

        for (const auto &point_light: point_lights) {
            result += glm::vec4(calc_point_light(point_light, v2f, normal, view_dir), 1.0);
        }

        for (const auto &spot_light: spot_lights) {
            result += glm::vec4(calc_spot_light(spot_light, v2f, normal, view_dir), 1.0);
        }
        return result;
    }

    glm::vec3 calc_dir_light(shared_ptr<direction_light> dir_light_info, const vertex2fragment v2f, glm::vec3 normal,
                             glm::vec3 view_dir) {
        glm::vec3 light_dir = -glm::normalize(dir_light_info->get_light_direction(v2f.world_pos));
        // diffuse shading
        float diff = std::fmax(glm::dot(normal, light_dir), 0.0);
        // specular shading
        glm::vec3 reflect_dir = glm::reflect(-light_dir, normal);
        float spec = std::pow(std::fmax(glm::dot(view_dir, reflect_dir), 0.0), _material->shininess);
        // combine results
        glm::vec3 albedo = glm::vec3(_material->get_diffuse(v2f.texcoord.x, v2f.texcoord.y));
        glm::vec3 ambient = dir_light_info->ambient * glm::vec3(albedo);
        glm::vec3 diffuse = dir_light_info->diffuse * diff * glm::vec3(albedo);
        glm::vec3 specular =
                dir_light_info->specular * spec * glm::vec3(_material->get_specular(v2f.texcoord.x, v2f.texcoord.y));
        return (ambient + diffuse + specular);
    }

    // calculates the color when using a point light.
    glm::vec3
    calc_point_light(shared_ptr<point_light> _light, const vertex2fragment v2f, glm::vec3 normal, glm::vec3 view_dir) {
        glm::vec3 light_dir = -_light->get_light_direction(v2f.world_pos);
        // diffuse shading
        float diff = std::fmax(glm::dot(normal, light_dir), 0.0);
        // specular shading
        glm::vec3 reflect_dir = glm::reflect(-light_dir, normal);
        float spec = std::pow(std::fmax(glm::dot(view_dir, reflect_dir), 0.0), _material->shininess);
        // attenuation
        float distance = glm::length(_light->position - glm::vec3(v2f.world_pos));
        float attenuation =
                1.0 / (_light->constant + _light->linear * distance + _light->quadratic * (distance * distance));
        // combine results
        glm::vec3 albedo = glm::vec3(_material->get_diffuse(v2f.texcoord.x, v2f.texcoord.y));
        glm::vec3 ambient = _light->ambient * glm::vec3(albedo);
        glm::vec3 diffuse = _light->diffuse * diff * glm::vec3(albedo);
        glm::vec3 specular =
                _light->specular * spec * glm::vec3(_material->get_specular(v2f.texcoord.x, v2f.texcoord.y));
        ambient *= attenuation;
        diffuse *= attenuation;
        specular *= attenuation;
        return (ambient + diffuse + specular);
    }

    // calculates the color when using a spot light.
    glm::vec3
    calc_spot_light(shared_ptr<spot_light> _light, const vertex2fragment v2f, glm::vec3 normal, glm::vec3 view_dir) {
        glm::vec3 light_dir = -_light->get_light_direction(v2f.world_pos);
        // diffuse shading
        float diff = std::fmax(glm::dot(normal, light_dir), 0.0);
        // specular shading
        glm::vec3 reflect_dir = glm::reflect(-light_dir, normal);
        float spec = std::pow(std::fmax(glm::dot(view_dir, reflect_dir), 0.0), _material->shininess);
        // attenuation
        float distance = glm::length(_light->position - glm::vec3(v2f.world_pos));
        float attenuation =
                1.0 / (_light->constant + _light->linear * distance + _light->quadratic * (distance * distance));
        // spotlight intensity
        float theta = glm::dot(light_dir, glm::normalize(-_light->direction));
        float epsilon = _light->cut_off - _light->outer_cut_off;
        float intensity = clamp((theta - _light->outer_cut_off) / epsilon, 0.0, 1.0);
        // combine results
        glm::vec3 albedo = glm::vec3(_material->get_diffuse(v2f.texcoord.x, v2f.texcoord.y));
        glm::vec3 ambient = _light->ambient * glm::vec3(albedo);
        glm::vec3 diffuse = _light->diffuse * diff * glm::vec3(albedo);
        glm::vec3 specular =
                _light->specular * spec * glm::vec3(_material->get_specular(v2f.texcoord.x, v2f.texcoord.y));
        ambient *= attenuation * intensity;
        diffuse *= attenuation * intensity;
        specular *= attenuation * intensity;
        return (ambient + diffuse + specular);
    }
};


#endif //RAYTRACING_SHADER_H
