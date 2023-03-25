#ifndef RAYTRACING_AABB_H
#define RAYTRACING_AABB_H

#include "glm/glm.hpp"
#include "ray.h"

class aabb {
public:
    aabb() {}

    aabb(const glm::vec3 &_min, const glm::vec3 &_max) {
        minimum = _min;
        maximum = _max;
    }

    glm::vec3 min() const { return minimum; }

    glm::vec3 max() const { return maximum; }

    bool hit(const ray &r, double t_min, double t_max) const {
        for (int i = 0; i < 3; i++) {
            auto invD = 1.0f / r.direction()[i];
            auto t0 = (min()[i] - r.origin()[i]) * invD;
            auto t1 = (max()[i] - r.origin()[i]) * invD;
            if (invD < 0.0f)
                std::swap(t0, t1);
            t_min = t0 > t_min ? t0 : t_min;
            t_max = t1 < t_max ? t1 : t_max;
            if (t_max <= t_min)
                return false;
        }
        return true;
    }

private:
    glm::vec3 minimum;
    glm::vec3 maximum;
};

aabb surrounding_box(aabb box0, aabb box1) {
    glm::vec3 small(fmin(box0.min().x, box1.min().x),
                    fmin(box0.min().y, box1.min().y),
                    fmin(box0.min().z, box1.min().z));

    glm::vec3 big(fmax(box0.max().x, box1.max().x),
                  fmax(box0.max().y, box1.max().y),
                  fmax(box0.max().z, box1.max().z));

    return aabb(small, big);
}


#endif //RAYTRACING_AABB_H
