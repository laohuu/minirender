#ifndef RAYTRACING_LIGHT_H
#define RAYTRACING_LIGHT_H

#include "glm/glm.hpp"

class light {
public:
    glm::vec3 position;
    glm::vec3 diffuse;
    glm::vec3 specular;
    glm::vec3 direction;
    glm::vec3 ambient;
    float intensity;

    light(const glm::vec3 &pos = glm::vec3(0, 0, 0),
          const glm::vec3 &diffuse = glm::vec3(1, 1, 1),
          const glm::vec3 &specular = glm::vec3(1, 1, 1),
          const glm::vec3 &dir = glm::vec3(0, -1, 0),
          const float &i = 1.0f, glm::vec3 _ambient = glm::vec3(0.05f, 0.05f, 0.05f)) : position(pos),
                                                                                        diffuse(diffuse),
                                                                                        specular(specular),
                                                                                        direction(dir),
                                                                                        intensity(i),
                                                                                        ambient(_ambient) {}

    ~light() {}

    virtual glm::vec3 get_light_direction(glm::vec3 pos) {
        return direction;
    }
};

class direction_light : public light {
public:
    direction_light(
            const glm::vec3 &dir = glm::normalize(glm::vec3(0, -1.f, -1.0f)),
            const glm::vec3 &diffuse = glm::vec3(1.0f, 1.0f, 1.0f),
            const glm::vec3 &specular = glm::vec3(0.5f, 0.5f, 0.5f),
            const float &i = 0.5f,
            glm::vec3 _ambient = glm::vec3(0.05f, 0.05f, 0.05f)) {
        this->diffuse = diffuse;
        this->direction = dir;
        this->specular = specular;
        position = glm::vec4(0.0f);
        intensity = i;
        ambient = _ambient;
    }

    ~direction_light() = default;

    glm::vec3 get_light_direction(glm::vec3 pos) override {
        return glm::normalize(direction);
    }
};

class point_light : public light {
public:
    float constant;
    float linear;
    float quadratic;

    point_light(
            glm::vec3 pos = glm::vec3(0, 2.f, 2.0f),
            glm::vec3 diffuse = glm::vec3(0.8f, 0.8f, 0.8f),
            glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3 _ambient = glm::vec3(0.05f, 0.05f, 0.05f),
            float _constant = 1.0f, float _linear = 0.09f, float _quadratic = 0.032f) : constant(_constant),
                                                                                        linear(_linear),
                                                                                        quadratic(_quadratic) {
        this->diffuse = diffuse;
        this->position = pos;
        this->specular = specular;
        position = glm::vec4(0.0f);
        ambient = _ambient;
    }

    ~point_light() = default;

    glm::vec3 get_light_direction(glm::vec3 pos) override {
        return glm::normalize(pos - position);
    }
};

class spot_light : public light {
public:
    float constant;
    float linear;
    float quadratic;
    float cut_off;
    float outer_cut_off;

    spot_light(
            glm::vec3 pos = glm::vec3(0, 0.f, 1.0f),
            glm::vec3 dir = glm::normalize(glm::vec3(0, 0.0f, -1.0f)),
            glm::vec3 diffuse = glm::vec3(0.8f, 0.8f, 0.8f),
            glm::vec3 specular = glm::vec3(1.0f, 1.0f, 1.0f),
            glm::vec3 _ambient = glm::vec3(0.1f, 0.1f, 0.1f),
            float _constant = 1.0f, float _linear = 0.09f, float _quadratic = 0.032f) : constant(_constant),
                                                                                        linear(_linear),
                                                                                        quadratic(_quadratic) {
        this->diffuse = diffuse;
        this->position = pos;
        this->specular = specular;
        this->direction = dir;
        position = glm::vec4(0.0f);
        ambient = _ambient;
        cut_off = glm::cos(glm::radians(50.5f));
        outer_cut_off = glm::cos(glm::radians(90.5f));
    }

    ~spot_light() = default;

    glm::vec3 get_light_direction(glm::vec3 pos) override {
        return glm::normalize(pos - position);
    }
};

#endif //RAYTRACING_LIGHT_H
