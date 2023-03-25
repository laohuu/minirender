#ifndef RAYTRACING_HITTABLE_H
#define RAYTRACING_HITTABLE_H

#include "glm/glm.hpp"
#include "ray.h"
#include "material.h"

struct hit_record {
    glm::vec3 p;
    glm::vec3 normal;
    shared_ptr<material> mat_ptr;
    float u;
    float v;
//    double t;

    bool front_face;

    inline void set_face_normal(const ray &r, const glm::vec3 &outward_normal) {
        front_face = dot(r.get_direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
public:
//    virtual bool hit(const ray &r, double t_min, double t_max, hit_record &rec) const = 0;

//    virtual bool bounding_box(double time0, double time1, aabb &output_box) const = 0;

};

#endif //RAYTRACING_HITTABLE_H
