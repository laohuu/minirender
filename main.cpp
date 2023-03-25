#include "utils.h"
#include "enable_stb_image.h"
#include "filesystem.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"
#include "framebuffer.h"
#include <iostream>
#include "model.h"
#include "rasterizer.h"
#include "camera.h"


glm::mat4 get_model_matrix(glm::vec3 world_pos, float angle) {
    glm::mat4 transform = glm::mat4(1.0f);
    transform = glm::translate(transform, world_pos);
    transform = glm::rotate(transform, angle, glm::vec3(0.0f, 0.0f, 1.0f));
    return transform;
}

int main() {
    // load models
    // ----------
    model our_model(FileSystem::getPath("resources/objects/Crate/Crate1.obj"));

    // Image
    const float aspect_ratio = 16.0 / 9.0;
    const int image_width = 1280;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    const int samples_per_pixel = 50;
    const int max_depth = 50;
    float eye_fov = 45.f;
    const int image_channel = 3;
    camera cam(glm::vec3(0, 0, 8.f), glm::vec3(0.f), eye_fov, aspect_ratio, 0.01f, 100.0f);

    glm::vec4 background_color(0.05f, 0.05f, 0.05f, 1.0f);

    rasterizer raster(image_width, image_height, image_channel);
    raster.clear_color_buffer(background_color);
    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(model, glm::vec3(0.0f, 0.0f, -5.0f));
    model = glm::rotate(model, glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::rotate(model, glm::radians(20.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(10.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//    model = glm::scale(model, glm::vec3(0.5f));

    glm::mat4 view = cam.get_view_matrix();
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect_ratio, 0.01f, 100.0f);

    raster.set_model_matrix(model);
    raster.set_view_matrix(view);
    raster.set_projection_matrix(projection);

    our_model.draw(raster);
    raster.ouput_image();

    std::cerr << "\nDone.\n";
    return 0;
}

