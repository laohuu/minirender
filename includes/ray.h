//
// Created by hanghai.hu on 2023/3/20.
//

#ifndef RAYTRACING_RAY_H
#define RAYTRACING_RAY_H

#include "glm/glm.hpp"

class ray {
public:
    ray() = default;

    ray(const glm::vec3 &origin, const glm::vec3 &direction, double time = 0.0)
            : orig(origin), dir(direction), tm(time) {}

    ray(const glm::vec3 &origin, const glm::vec3 &direction)
            : orig(origin), dir(direction) {}

    glm::vec3 at(double t) const {
        return orig + dir * tm;
    }

    glm::vec3 origin() const { return orig; }
    glm::vec3 direction() const { return dir; }

    double time() const { return tm; }

public:
    glm::vec3 dir;
    glm::vec3 orig;
    float tm;
};

#endif //RAYTRACING_RAY_H
