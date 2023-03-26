#ifndef RAYTRACING_MATERIAL_H
#define RAYTRACING_MATERIAL_H

#include "texture.h"

class material {
public:
    glm::vec4 albedo_value{};
    float shininess = 32.0f;

    virtual glm::vec4 get_diffuse(double u, double v) const {
        return albedo_value;
    }

    virtual glm::vec4 get_specular(double u, double v) const {
        return glm::vec4(0.0f);
    }

    virtual glm::vec4 get_normal(double u, double v) const {
        return glm::vec4(0.0f);
    }
};

class lambertian : public material {
public:
    shared_ptr<texture> diffuse;
    shared_ptr<texture> specular;
    shared_ptr<texture> normal;
public:
    lambertian(const glm::vec4 &color) : diffuse(make_shared<solid_color>(color)), specular(nullptr), normal(nullptr) {}

    lambertian(shared_ptr<texture> _diffuse) : diffuse(_diffuse) {}

    lambertian(shared_ptr<texture> _diffuse, shared_ptr<texture> _specular) : diffuse(_diffuse), specular(_specular) {}

    lambertian(shared_ptr<texture> _diffuse, shared_ptr<texture> _specular, shared_ptr<texture> _normal) : diffuse(
            _diffuse), specular(_specular), normal(_normal) {}

    virtual glm::vec4 get_specular(double u, double v) const override {
        if (specular == nullptr)
            return glm::vec4(0.0f);
        return specular->get_value(u, v);
    }

    virtual glm::vec4 get_diffuse(double u, double v) const override {
        if (diffuse == nullptr)
            return glm::vec4(0.0f);
        return diffuse->get_value(u, v);
    }

    virtual glm::vec4 get_normal(double u, double v) const override {
        if (normal == nullptr)
            return glm::vec4(0.0f);
        return normal->get_value(u, v);;
    }
};


#endif //RAYTRACING_MATERIAL_H
