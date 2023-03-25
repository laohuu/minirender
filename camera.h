#ifndef RAYTRACING_CAMERA_H
#define RAYTRACING_CAMERA_H


#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>

class camera {
public:
    camera(
            glm::vec3 _position,
            glm::vec3 _target,
            float vfov,
            float aspect_ratio,
            float z_near,
            float z_far,
            glm::vec3 vup = glm::vec3(0, 1, 0)
    ) : position(_position), target(_target), fov(vfov), aspect_ratio(aspect_ratio), z_near(z_near), z_far(z_far) {
        float theta = glm::radians(vfov);
        float h = glm::tan(theta / 2);
        float viewport_height = 2.0 * h;
        float viewport_width = aspect_ratio * viewport_height;

        w = glm::normalize(position - target);
        u = glm::normalize(glm::cross(vup, w));
        v = glm::cross(w, u);

        horizontal = viewport_width * u;
        vertical = viewport_height * v;
        lower_left_corner = position - horizontal / 2.0f - vertical / 2.0f - w;
    }

//    ray get_ray(float s, float t) const {
//        return ray(origin, lower_left_corner + s * horizontal + t * vertical - origin);
//    }

    glm::mat4 get_view_matrix() {
        glm::mat4 view = glm::mat4(1.0f);
//        view = glm::translate(view, position);
//        glm::lookAt(position, target, {0.0f, 1.0f, 0.0f})
        return glm::lookAt(position, target, {0.0f, 1.0f, 0.0f});
//        return glm::translate(view, glm::vec3(0.0f, 0.0f, -8.0f));
    }

    glm::mat4 get_projection_matrix() {
        return glm::perspective(glm::radians(fov), aspect_ratio, z_near, z_far);
    }

private:
    glm::vec3 position;
    glm::vec3 target;
    glm::vec3 lower_left_corner;
    glm::vec3 horizontal;
    glm::vec3 vertical;
    glm::vec3 u, v, w;
    float fov;
    float aspect_ratio;
    float z_near;
    float z_far;
    double lens_radius;
};

#endif //RAYTRACING_CAMERA_H
