#ifndef RAYTRACING_TRANSFORMATION_H
#define RAYTRACING_TRANSFORMATION_H

#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "utils.h"
#include <iostream>

using namespace std;

glm::mat4 get_view_matrix(glm::vec3 cam_pos) {
    glm::mat4 view_matrix(1.0f);

    glm::mat4 trans(1.0f);
    trans = glm::translate(trans, cam_pos);//设置平移矩阵
    view_matrix = trans * view_matrix;
    return view_matrix;
}

glm::mat4 get_model_matrix(float rotation_angle) {
    glm::mat4 model_matrix(1.0f);

    glm::mat4 rotate;

    rotate = glm::transpose(
            glm::mat4(std::cos(rotation_angle / 180.0 * pi), -std::sin(rotation_angle / 180.0 * pi), 0, 0,
                      std::sin(rotation_angle / 180.0 * pi), std::cos(rotation_angle / 180.0 * pi), 0, 0,
                      0, 0, 1, 0,
                      0, 0, 0, 1));

    model_matrix = rotate * model_matrix;
    return model_matrix;
}


glm::mat4 get_projection_matrix(float eye_fov, float aspect_ratio, float zNear, float zFar) {
    glm::mat4 projection(1.0f);

    glm::mat4 persp_to_ortho;
    persp_to_ortho = glm::mat4(zNear, 0, 0, 0,
                               0, zNear, 0, 0,
                               0, 0, zNear + zFar, -zNear * zFar,
                               0, 0, 1, 0);

    float t = -zNear * std::tan(eye_fov / 2.0);
    float r = t * aspect_ratio;
    float l = -r;
    float b = -t;

    glm::mat4 translate;

    translate = glm::mat4(1, 0, 0, 0,
                          0, 1, 0, 0,
                          0, 0, 1, -(zNear + zFar) / 2.0,
                          0, 0, 0, 1);

    glm::mat4 scale;

    scale = glm::mat4(2 / (r - l), 0, 0, 0,
                      0, 2 / (t - b), 0, 0,
                      0, 0, 2 / (zNear - zFar), 0,
                      0, 0, 0, 1);

    projection = scale * translate * persp_to_ortho * projection;
    return projection;
}

#endif //RAYTRACING_TRANSFORMATION_H
