//
// Created by hanghai.hu on 2023/3/20.
//

#ifndef RAYTRACING_RAY_H
#define RAYTRACING_RAY_H

#include "glm/glm.hpp"

class ray {
private:
    glm::vec3 direction;
    glm::vec3 position;
public:

    ray() = default;

    ray(const glm::vec3 &origin, const glm::vec3 &direction)
            : position(origin), direction(direction) {}

    glm::vec3 get_position_by_time(float t) const {
        return position + direction * t;
    }

    glm::vec3 get_position() const { return position; }
    glm::vec3 get_direction() const { return direction; }
};

#endif //RAYTRACING_RAY_H
